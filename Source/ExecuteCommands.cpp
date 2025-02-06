
#include "ExecuteCommands.hh"
#include <dirent.h>
#include <fstream>

extern deque<MessageHeader> responseDeque;
extern mutex mtx;

/*********************************************************************
 * @fn      		  - executeCmd
 * @brief             - This function handles the different command execution 
 *                      requested by client
 * @param[in]         - int clientSocket, MessageHeader in
 * @return            - none
 * @Note              -
 *********************************************************************/
void executeCmd(int clientSocket, MessageHeader in)
{
        command_e receivedCommand = in.getCommand();
        string resp = "";
        switch (receivedCommand)
        {
            default:
            {

            }
            case CMD_GET_PROCESS:
            {//get-process
                resp = execGetProcess();
                break;
            }
            case CMD_GET_MEMORY:
            {//get-mem
                vector<int> pids;
                if(in.checkIsPid())
                    pids.push_back(in.getProcessId());
                else
                    pids = getPIDsByName(in.getProcessName()); 

                resp = execGetMemoryUsage(pids);
                break;
            }

            case CMD_GET_CPU_USAGE:
            {//get-cpu-usage
                break;
            }

            case CMD_GET_PORT_USED:
            {//get-ports-used
                break;
            }

            case CMD_KILL_PROCESS:
            {//kill
                break;
            }
            case CMD_EXIT:
            {//exit

                break;
            }

            case CMD_MAX:
            { //HEARTBEAT or INVALID

                if(MSG_HEARTBEAT == in.getMsgType())
                {
                    
                }
                break;
            }

        }
        if(CMD_MAX != receivedCommand)
            prepareAndTx(clientSocket, resp);
}

/*********************************************************************
 * @fn      		  - execGetProcess()
 * @brief             - This function reads the list of all the running process
 * @param[in]         - none
 * @return            - string
 * @Note              -
 *********************************************************************/
string execGetProcess()
{
    DIR* proc_dir = opendir("/proc");
    if (!proc_dir) {
        cerr << "Failed to open /proc directory." << endl;
        return NULL;
    }
    string resp = "";
    dirent* entry;
    
    while ((entry = readdir(proc_dir)) != nullptr) 
    {
        if (isdigit(entry->d_name[0])) {
            string pid_str = entry->d_name;
            string cmdline_path = "/proc/" + pid_str + "/cmdline";
            ifstream cmdline_file(cmdline_path);
            string cmdline;

            
             // Get the command line
            getline(cmdline_file, cmdline, '\0');
            
            string temp = pid_str + " : " + cmdline + "\n";
            resp += temp;
            cout.flush();
        }
    }

    closedir(proc_dir);
    return resp;
}

/*********************************************************************
 * @fn      		  - execGetMemoryUsage()
 * @brief             - This function calculates the memory used by the pid given in
 *                      argument
 * @param[in]         - vector<int> pids
 * @return            - string
 * @Note              -
 *********************************************************************/
string execGetMemoryUsage(vector<int> pids)
{
    string resp = "";
    for(int pid : pids)
    {
        string path = "/proc/" + to_string(pid) + "/status";
        ifstream status_file(path);
        
        if (!status_file.is_open()) 
        {
            cerr << "Process with PID " << pid << " not found or access denied" << endl;
            return NULL;
        }

        string line;
        long vm_rss = -1, vm_size = -1;

        while (getline(status_file, line)) 
        {
            if (line.compare(0, 6, "VmRSS:") == 0) 
            {
                stringstream ss(line.substr(6));
                ss >> vm_rss;
            } 
            else if (line.compare(0, 7, "VmSize:") == 0) 
            {
                stringstream ss(line.substr(7));
                ss >> vm_size;
            }
        }
        resp += "Memory usage for PID " + to_string(pid) + ":\n";
        cout << "Memory usage for PID " << pid << ":" << endl;
        if (vm_rss != -1) 
        {
            resp += "Physical Memory (VmRSS): " + to_string(vm_rss) + " KB (" + to_string(vm_rss / 1024.0) + " MB)\n";
            cout << "Physical Memory (VmRSS): " << vm_rss << " KB (" 
                << vm_rss / 1024.0 << " MB)" << endl;
        } 
        else 
        {
            resp += "VmRSS information not available\n";
            cout << "VmRSS information not available" << endl;
        }

        if (vm_size != -1) 
        {
            resp += "Virtual Memory (VmSize): " + to_string(vm_size) + " KB (" + to_string(vm_size / 1024.0) + " MB)\n";
            cout << "Virtual Memory (VmSize): " << vm_size << " KB (" 
                << vm_size / 1024.0 << " MB)" << endl;

        } 
        else 
        {
            resp += "VmSize information not available\n";
            cout << "VmSize information not available" << endl;
        }
        resp += "\n";
    }
    return resp;
}


/*********************************************************************
 * @fn      		  - execGetMemoryUsage()
 * @brief             - This function is used to get PIDs associated with a process name
 *                      argument
 * @param[in]         - const string& processName
 * @return            - vector<int>
 * @Note              -
 *********************************************************************/
vector<int> getPIDsByName(const string& processName) 
{
    vector<int> pids;
    DIR* dir = opendir("/proc");
    if (!dir) 
    {
        cerr << "Failed to open /proc directory." << endl;
        return pids;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) 
    {
        // Check if the entry is a directory and represents a PID
        if(entry->d_type == DT_DIR && isdigit(entry->d_name[0])) 
        {
            int pid = stoi(entry->d_name);

            // Read the process name from /proc/[PID]/comm
            string commPath = string("/proc/") + entry->d_name + "/comm";
            ifstream commFile(commPath);
            if (commFile.is_open()) 
            {
                string name;
                getline(commFile, name);
                if (name == processName) 
                {
                    pids.push_back(pid);
                }
                commFile.close();
            }
        }
    }
    closedir(dir);
    return pids;
}

/*********************************************************************
 * @fn      		  - prepareAndTx()
 * @brief             - This function prepares the response according to 
 *                      MESSAGE_SIZE in cunks and adds into responseDeque to 
 *                      send to client
 * @param[in]         - int clientSocket,string resp
 * @return            - none
 * @Note              -
 *********************************************************************/
void prepareAndTx(int clientSocket,string resp)
{
    if("" == resp)
    {
        cout << "No Responce " << endl;
        return;
    }

    for(size_t i = 0; i < resp.length(); i += MESSAGE_SIZE) 
    {
        MessageHeader out;
        string chunk = resp.substr(i, MESSAGE_SIZE);
        
        out.setResponse(APPTYPE_SERVER, MSG_TYPE_RESPONSE, clientSocket ,(i/MESSAGE_SIZE + 1), chunk);
        //out.printResponse();
        lock_guard<mutex> guard(mtx);
        responseDeque.push_back(out);
    }
    MessageHeader endout;
    endout.setResponse(APPTYPE_SERVER, MSG_TYPE_END_OF_RESPONSE,clientSocket,-1,NULL);
    responseDeque.push_back(endout);

}
