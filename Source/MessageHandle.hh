#ifndef MESSSAGE_HANDLE_H
#define MESSSAGE_HANDLE_H

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "RemoteManagement.hh"

#define		ENUM(ENUM, STRING)		ENUM,
#define		STRING(ENUM, STRING)	STRING,

#define CMD_GENERATOR(ARG)                                              \
    ARG(CMD_GET_PROCESS,"get-process")                                  \
    ARG(CMD_GET_MEMORY,"get-mem")                                       \
    ARG(CMD_GET_CPU_USAGE,"get-cpu-usage")                              \
    ARG(CMD_GET_PORT_USED,"get-ports-used")                             \
    ARG(CMD_KILL_PROCESS,"kill")                                        \
    ARG(CMD_RESTART_PROCESS,"restart-process")                          \
    ARG(CMD_EXIT,"exit")                                                \


#define MSG_GENERATOR(ARG)                                              \
    ARG(MSG_TYPE_CMD,"MSG_TYPE_CMD")                                    \
    ARG(MSG_TYPE_RESPONSE,"MSG_TYPE_RESPONSE")                          \
    ARG(MSG_TYPE_END_OF_RESPONSE,"MSG_TYPE_END_OF_RESPONSE")            \
    ARG(MSG_HEARTBEAT,"MSG_HEARTBEAT")                                  \
    ARG(MSG_INVALID,"")                                  \


typedef enum
{
    CMD_GENERATOR(ENUM)
    CMD_MAX,
} command_e;

typedef enum
{
    MSG_GENERATOR(ENUM)
    MSG_TYPE_MAX,
} msgType_e;

typedef struct
{
    int socket;
    int sequenceNum;
    char msg[MESSAGE_SIZE+1];
} response_t;

struct MessageHeader
{
private:
    appType_e selfInfo;
    int messageSize;
    msgType_e msgType;
    command_e command;
    bool isPid;
    variant<int, array<char, MESSAGE_SIZE>> pidOrProccessNameVariant;
    response_t response;

public:
    MessageHeader();
    void setSelfInfo(appType_e iSelfInfo);
    void setMsgType(msgType_e iMsgType);
    void setCommand(command_e iCommand);
    void setIsPid(bool isPid);
    void setpidOrProccessName(int iPis, string iProcessName);
    void setMessageHandlerInfo(string msg);
    void setResponse(appType_e type, msgType_e mType, int clientSocket, int sequenceNum,string resp);

    void processIdentifier(const string &identifier);
    bool parseArgumentAndPrepareCommand(const vector<string> &args);

    inline int getSocketIdToSendResponse() { return this->response.socket; }
    inline msgType_e getMsgType() { return this->msgType; }
    inline command_e getCommand() { return this->command; }
    bool checkIsPid();

    void printHeader();
    void printResponse();

    void MessageHeartBeat(appType_e app);
    string getProcessName();
    int getProcessId();

    ~MessageHeader();
};

#endif
