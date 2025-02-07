#include "NetworkValidator.hh"
#include <thread>
#include "MessageHandle.hh"

extern appType_e appType;
extern deque<MessageHeader> responseDeque;

/*********************************************************************
 * @fn      		  - NetworkValidator() [parameterised constructor]
 * @brief             - This constructor  initialise the object of the class
 *                      with provided value in arguments
 * @param[in]         - int clientSocket
 * @return            - none
 * @Note              - Constructor Overloading (by different number of arguments)
 *********************************************************************/
NetworkValidator::NetworkValidator(int clientSocket) : clientSocket(clientSocket), heartbeatInterval(HEARTBEAT_TIMEOUT) ,status(false){}

/* In Constructor initialise variables of class */
void NetworkValidator::StartValidator()
{
    // Start the heartbeat thread
    thread heartbeatThread(&NetworkValidator::SendHeartbeat, this);
    heartbeatThread.detach();
    this -> status = true;
}

/*********************************************************************
 * @fn      		  - SendHeartbeat() 
 * @brief             - This function starts a thread to send heartbeat to server
 * @param[in]         - none
 * @return            - none
 * @Note              - 
 *********************************************************************/
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

/*********************************************************************
 * @fn      		  - initHeartBeatTimer() 
 * @brief             - This function starts the heartbeat received checek timer for server
 * @param[in]         - none
 * @return            - none
 * @Note              - 
 *********************************************************************/
void NetworkValidator::initHeartBeatTimer()
{
    // Set timeout for receiving heartbeats
    struct timeval timeout;
    timeout.tv_sec = heartbeatInterval;
    timeout.tv_usec = 0;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);
}

/*********************************************************************
 * @fn      		  - StopValidator() 
 * @brief             - This function stops the heartbeat chcek thread
 * @param[in]         - none
 * @return            - none
 * @Note              - 
 *********************************************************************/
void NetworkValidator::StopValidator()
{
    status = false;
}
