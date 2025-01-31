#ifndef NETWORK_SETTINGS_H
#define NETWORK_SETTINGS_H

#include <vector>
#include <thread>
#include "RemoteManagement.hh"

#define CMDPROMPT "RemoteManagement> "

class NetworkSettings
{
private:
    int sock;
    struct sockaddr_in address;
    static const int PORT = 8080;
    static const int BUFFER_SIZE = 1024;
    vector<thread> clientThreads;
    void handleClient(int clientSocket);

public:
    NetworkSettings();
    bool initializeAsServer();
    int initializeAsClient(const char *serverIP);
    void runServer();
    void runClient();
    inline int getClientSocketId() { return sock; }
    ~NetworkSettings();
};

#endif
