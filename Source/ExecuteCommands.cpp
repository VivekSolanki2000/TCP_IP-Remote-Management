
#include "ExecuteCommands.hh"
#include <dirent.h>
#include <fstream>

extern deque<MessageHeader> responseDeque;
extern mutex mtx;
extern string msgStr[MSG_TYPE_MAX];
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
        vector<int> pids;
        

        switch (receivedCommand)
        {
            case CMD_GET_PROCESS:
            {//get-process
                resp = execGetProcess();
                break;
            }
            case CMD_GET_MEMORY:
            {//get-mem
                if(in.checkIsPid())
                    pids.push_back(in.getProcessId());
                else
                    pids = getPIDsByName(in.getProcessName()); 
                resp = execGetMemoryUsage(pids);
                break;
            }

            case CMD_GET_CPU_USAGE:
            {//get-cpu-usage
                if(in.checkIsPid())
                    pids.push_back(in.getProcessId());
                else
                    pids = getPIDsByName(in.getProcessName()); 
                resp = execgetCPUUsage(pids);
                break;
            }

            case CMD_GET_PORT_USED:
            {//get-ports-used
                if(in.checkIsPid())
                    pids.push_back(in.getProcessId());
                else
                    pids = getPIDsByName(in.getProcessName()); 
                resp = execUsedPorts(pids);
                break;
            }

            case CMD_KILL_PROCESS:
            {//kill
                if(in.checkIsPid())
                    pids.push_back(in.getProcessId());
                else
                    pids = getPIDsByName(in.getProcessName()); 
                resp = execkillProcess(pids);
                break;
            }
            case CMD_RESTART_PROCESS:
            {//restart-process
                if(in.checkIsPid())
                    pids.push_back(in.getProcessId());
                else
                    pids = getPIDsByName(in.getProcessName()); 
                resp = execRestartProcess(pids);
                break;
            }

            case CMD_EXIT:
            {
                //future-use
            }

            case CMD_MAX:
            { //HEARTBEAT or INVALID

                if(MSG_HEARTBEAT == in.getMsgType())
                {
                    //future-use
                }
                break;
            }
            
            default:
            {
                //future-use
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
    string resp = msgStr[MSG_INVALID];
    if (!proc_dir) {
        cerr << "Failed to open /proc directory." << endl;
        resp += "No process available\n";
        return resp;
    }
    
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
    string resp = msgStr[MSG_INVALID];
    for(int pid : pids)
    {
        string path = "/proc/" + to_string(pid) + "/status";
        ifstream status_file(path);
        
        if (!status_file.is_open()) 
        {
            cerr << "Process with PID " << pid << " not found or access denied" << endl;
            resp += "Process with PID" + to_string(pid) + " not found or access denied\n";
            continue;
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
        resp += "\n\0";
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

// Function to get CPU usage of a process by PID
string execgetCPUUsage(vector<int> pids) {

    string resp = msgStr[MSG_INVALID];
    for(int pid:pids)
    {
        string statPath = "/proc/" + to_string(pid) + "/stat";
        ifstream statFile(statPath);

        if (!statFile.is_open()) {
            cerr << "Failed to open /proc/" << pid << "/stat. Process may not exist or access denied." << endl;
            resp += "PID[" + to_string(pid) + "]: Process may not exist or access denied.\n";
            continue;
        }

        // Read the entire line from /proc/[PID]/stat
        string line;
        getline(statFile, line);
        statFile.close();

        // Extract process CPU times (utime and stime)
        istringstream iss(line);
        string token;
        vector<string> tokens;
        while (iss >> token) {
            tokens.push_back(token);
        }

        // utime (14th field) and stime (15th field) in /proc/[PID]/stat
        long utime = stol(tokens[13]);
        long stime = stol(tokens[14]);

        // Total process CPU time (in clock ticks)
        long processTime = utime + stime;

        // Read total CPU time from /proc/stat
        ifstream statGlobalFile("/proc/stat");
        if (!statGlobalFile.is_open()) {
            resp += "PID[" + to_string(pid) + "]: access denied.\n";
            continue;;
        }

        string cpuLine;
        getline(statGlobalFile, cpuLine);
        statGlobalFile.close();

        // Extract total CPU time from /proc/stat
        istringstream cpuIss(cpuLine);
        string cpuLabel;
        cpuIss >> cpuLabel; // Skip the "cpu" label
        long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
        cpuIss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

        // Total system CPU time (in clock ticks)
        long totalCPUTime = user + nice + system + idle + iowait + irq + softirq + steal;

        // Calculate CPU usage percentage
        static long prevProcessTime = 0;
        static long prevTotalCPUTime = 0;

        long processTimeDiff = processTime - prevProcessTime;
        long totalCPUTimeDiff = totalCPUTime - prevTotalCPUTime;

        prevProcessTime = processTime;
        prevTotalCPUTime = totalCPUTime;

        if (totalCPUTimeDiff == 0) {
           resp += "Memory usage for PID " + to_string(pid) + ": " +  to_string(0.0) + "\n";
        }
        else{
            double cpuUsage = 100.0 * processTimeDiff / totalCPUTimeDiff;
            resp += "Memory usage for PID " + to_string(pid) + ": " +  to_string(cpuUsage) + "\n";
        }
    }
    resp += "\0";
    return resp;
}

// Function to convert a hexadecimal IP address to a human-readable format
string hexToIP(const string& hex) {
    if (hex.size() != 8) {
        return "Invalid IP";
    }

    // Convert hex to 32-bit integer
    unsigned int ipInt = stoul(hex, nullptr, 16);

    // Convert to network byte order
    ipInt = ntohl(ipInt);

    // Convert to dotted-decimal notation
    struct in_addr addr;
    addr.s_addr = ipInt;
    return string(inet_ntoa(addr));
}

// Function to convert a hexadecimal port to a decimal port
int hexToPort(const string& hex) {
    return stoul(hex, nullptr, 16);
}

// Function to get used ports by PID
string execUsedPorts(vector<int> pids) 
{
    string resp = msgStr[MSG_INVALID];
    for(int pid:pids)
    {
        string tcpPath = "/proc/" + to_string(pid) + "/net/tcp";
        string udpPath = "/proc/" + to_string(pid) + "/net/udp";

        cout << "Used ports for PID " << pid << ":" << endl;
        resp += "Used ports for PID " + to_string(pid) + ":\n";
        // Read TCP ports
        ifstream tcpFile(tcpPath);
        if (tcpFile.is_open()) {
            string line;
            getline(tcpFile, line); // Skip the header line
            while (getline(tcpFile, line)) {
                istringstream iss(line);
                string token;
                vector<string> tokens;
                while (iss >> token) {
                    tokens.push_back(token);
                }

                if (tokens.size() >= 10) {
                    string localAddress = tokens[1];
                    size_t colonPos = localAddress.find(':');
                    string ipHex = localAddress.substr(0, colonPos);
                    string portHex = localAddress.substr(colonPos + 1);

                    string ip = hexToIP(ipHex);
                    int port = hexToPort(portHex);

                    cout << "TCP: " << ip << ":" << port << endl;
                    resp += "TCP: " + ip + ":" + to_string(port) + "\n";
                }
            }
            tcpFile.close();
        } else {
            cerr << "Failed to open " << tcpPath << endl;
            resp += "No TCP port Present\n";
        }

        // Read UDP ports
        ifstream udpFile(udpPath);
        if (udpFile.is_open()) {
            string line;
            getline(udpFile, line); // Skip the header line
            while (getline(udpFile, line)) {
                istringstream iss(line);
                string token;
                vector<string> tokens;
                while (iss >> token) {
                    tokens.push_back(token);
                }

                if (tokens.size() >= 10) {
                    string localAddress = tokens[1];
                    size_t colonPos = localAddress.find(':');
                    string ipHex = localAddress.substr(0, colonPos);
                    string portHex = localAddress.substr(colonPos + 1);

                    string ip = hexToIP(ipHex);
                    int port = hexToPort(portHex);

                    cout << "UDP: " << ip << ":" << port << endl;
                    resp += "UDP: " + ip + ":" + to_string(port) + "\n";
                }
            }
            udpFile.close();
        } else {
            cerr << "Failed to open " << udpPath << endl;
            resp += "No UDP port Present\n";
        }
        resp += "\n";
    }
    return resp;
}

// Function to get the executable path of a process by PID
string getExecutablePath(int pid) {
    char path[PATH_MAX];
    string exePath = "/proc/" + to_string(pid) + "/exe";
    ssize_t len = readlink(exePath.c_str(), path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        return string(path);
    } else {
        cerr << "Failed to get executable path for PID " << pid << endl;
        return "";
    }
}

// Function to start a process by executable path
bool startProcess(const string& executablePath) {
    int result = system((executablePath + " &").c_str()); // Run in background
    if (result == 0) {
        cout << "Successfully restarted process: " << executablePath << endl;
        return true;
    } else {
        cerr << "Failed to restart process: " << executablePath << endl;
        return false;
    }
}

string execRestartProcess(vector<int> pids) {
    string resp = msgStr[MSG_INVALID];
    for(int pid : pids)
    {
        string executablePath = getExecutablePath(pid);
        if (executablePath.empty()) {
            resp += "PID[" + to_string(pid) + "] :Restart not allowed\n";
            continue;
        }

        if (kill(pid, SIGTERM) == 0) {
            // Wait for the process to terminate
            sleep(2); // Adjust the delay as needed
            if(startProcess(executablePath))
                resp += "PID[" + to_string(pid) + "] :Restart Success\n";
            else
                resp += "PID[" + to_string(pid) + "] :Restart Failed\n";
        }
    }
    return resp;
}

string execkillProcess(vector<int> pids) {
    string resp = msgStr[MSG_INVALID];
    for(int pid : pids)
    {
        if (kill(pid, SIGTERM) == 0) {
            cout << "Successfully sent signal " << SIGTERM << " to PID " << pid << endl;
            resp += "Successfully Killed PID[" + to_string(pid) + "]\n";
        } else {
            cerr << "Failed to send signal " << SIGTERM << " to PID " << pid << endl;
            resp += "Failed to Killed PID[" + to_string(pid) + "]\n";
        }
    }
    return resp;
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
    if(msgStr[MSG_INVALID] == resp)
    {
        cout << "No Responce " << endl;
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
    endout.setResponse(APPTYPE_SERVER, MSG_TYPE_END_OF_RESPONSE,clientSocket,-1, msgStr[MSG_INVALID]);
    responseDeque.push_back(endout);

}
