#include "RemoteManagement.hh"


#define HEARTBEAT_TIMEOUT 5

class NetworkValidator
{
    int clientSocket;
    int heartbeatInterval;
    bool status;

    public:
    NetworkValidator(int clientSocket);
    void StartValidator();
    void SendHeartbeat();
    void initHeartBeatTimer();
    void StopValidator();

};