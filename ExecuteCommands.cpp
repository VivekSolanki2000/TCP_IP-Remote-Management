
#include "ExecuteCommands.hh"
#include <dirent.h>
#include <fstream>

extern deque<MessageHeader> responseDeque;

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

        }
        prepareAndTx(clientSocket, resp);
}

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
        
        out.setResponse(APPTYPE_SERVER,clientSocket ,(i/MESSAGE_SIZE + 1), chunk);
        out.printResponse();
        responseDeque.push_back(out);
    }
}