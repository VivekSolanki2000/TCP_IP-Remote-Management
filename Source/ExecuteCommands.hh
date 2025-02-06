#ifndef CMD_EXECUTE_H
#define CMD_EXECUTE_H


#include "RemoteManagement.hh"
#include "MessageHandle.hh"

string execGetProcess();
string execGetMemoryUsage(vector<int> pids);
string execgetCPUUsage(vector<int> pids);
string execUsedPorts(vector<int> pids);
string execkillProcess(vector<int> pids);
string execRestartProcess(vector<int> pids);
string getExecutablePath(int pid);
bool startProcess(const string& executablePath) ;
string hexToIP(const string& hex);
int hexToPort(const string& hex);
vector<int> getPIDsByName(const string& processName);
void executeCmd(int clientSocket, MessageHeader in);
void prepareAndTx(int clientSocket,string resp);


#endif