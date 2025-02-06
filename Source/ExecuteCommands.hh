#ifndef CMD_EXECUTE_H
#define CMD_EXECUTE_H


#include "RemoteManagement.hh"
#include "MessageHandle.hh"

string execGetProcess();
string execGetMemoryUsage(vector<int> pids);
vector<int> getPIDsByName(const string& processName);
void executeCmd(int clientSocket, MessageHeader in);
void prepareAndTx(int clientSocket,string resp);

#endif