#include "MessageHandle.hh"

MessageHeader::MessageHeader()
{
    selfInfo = APPTYPE_MAX;
    messageSize = 0;
    command = CMD_MAX;
    isPid = 0;
    pidOrProccessNameVariant = 0;
}

bool MessageHeader::checkIsPid()
{
    return isPid;
}

bool MessageHeader::parseMessageHandleInfo(MessageHeader iMessageHandle)
{
    bool status = false;

    return status;
}

void MessageHeader::setSelfInfo(appType_e iSelfInfo)
{
    if (iSelfInfo == APPTYPE_SERVER || iSelfInfo == APPTYPE_CLIENT)
        selfInfo = iSelfInfo;
}
void MessageHeader::setMsgType(msgType_e iMsgType)
{
    if (iMsgType >= MSG_TYPE_CMD && iMsgType <= MSG_TYPE_MAX)
        msgType = iMsgType;
}

void MessageHeader::setCommand(command_e iCommand)
{
    if (iCommand >= CMD_GET_PID && iCommand <= CMD_MAX)
        command = iCommand;
}

void MessageHeader::setIsPis(bool iIsPid)
{
    isPid = iIsPid;
}

void MessageHeader::setpidOrProccessName(int iPid, string iProcessName)
{
    pidOrProccessNameVariant = iPid;
 //   pidOrProccessNameVariant = iProcessName;
}

void MessageHeader::setMessageHandlerInfo(string msg)
{
    this->selfInfo = APPTYPE_CLIENT;
    this->msgType = MSG_TYPE_CMD;
    this->command = CMD_GET_PID;
    this->isPid = false;

    array<char, MESSAGE_SIZE> charArray{};
    copy_n(msg.begin(), min(msg.length(), static_cast<size_t>(99)), charArray.data());
    charArray[99] = '\0';

    this->pidOrProccessNameVariant = charArray;
}

void MessageHeader::printResponse()
{
    cout << "test\n";
    cout << this->selfInfo << endl;
    cout << this->msgType << endl;
    cout << this->command << endl;
    cout << this->isPid << endl;
    // cout << (get<int>(this->pidOrProccessNameVariant)) << endl;
    const auto& arr = std::get<std::array<char, MESSAGE_SIZE>>(pidOrProccessNameVariant);
    cout << arr.data() << endl;
    cout.flush();
}

void response_t::setResponse(int iSocket)
{
    this->msgType = MSG_TYPE_RESPONSE;
    this->socket = iSocket;
    this->sequenceNum = 1;
    string msg = "Response from server" + to_string(this->sequenceNum);
    strcpy(this->msg,msg.c_str());
}

MessageHeader::~MessageHeader()
{
}
