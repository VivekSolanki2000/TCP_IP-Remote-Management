

#include "NetworkValidator.hh"
#include <thread>
#include "MessageHandle.hh"
extern appType_e appType;
extern deque<MessageHeader> responseDeque;
NetworkValidator::NetworkValidator(int clientSocket) : clientSocket(clientSocket), heartbeatInterval(HEARTBEAT_TIMEOUT) ,status(false){}

void NetworkValidator::StartValidator()
{
    // Start the heartbeat thread
    thread heartbeatThread(&NetworkValidator::SendHeartbeat, this);
    heartbeatThread.detach();
    this -> status = true;
}

void NetworkValidator::SendHeartbeat()
{
    while (status)
    {
        MessageHeader HeartBeatMessage;
        HeartBeatMessage.MessageHeartBeat(appType);
        //responseDeque.push_back(HeartBeatMessage);
        send(clientSocket, &HeartBeatMessage, sizeof(HeartBeatMessage), 0);
        this_thread::sleep_for(chrono::milliseconds(HEARTBEAT_TIMEOUT*1000));
    }
}

void NetworkValidator::initHeartBeatTimer()
{
    // Set timeout for receiving heartbeats
    struct timeval timeout;
    timeout.tv_sec = heartbeatInterval;
    timeout.tv_usec = 0;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);
}

void NetworkValidator::StopValidator()
{
    status = false;
}