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
    if (iPid == -1)
    {
        cout << "test \n";
        array<char, MESSAGE_SIZE> charArray{};
        copy_n(iProcessName.begin(), min(iProcessName.length(), static_cast<size_t>(MESSAGE_SIZE - 1)), charArray.data());
        charArray[MESSAGE_SIZE - 1] = '\0';

        this->pidOrProccessNameVariant = charArray;
    }
    else
    {
        cout << "test 2\n";
        pidOrProccessNameVariant = iPid;
    }
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

void MessageHeader::printHeader()
{
    cout << "selfInfo: " << this->selfInfo << endl;
    cout << "msgType: " << this->msgType << endl;
    cout << "command: " << this->command << endl;
    cout << "isPid: " << this->isPid << endl;

    if (this->isPid == 0 && this->command != CMD_GET_PID)
    {
        if (std::holds_alternative<std::array<char, MESSAGE_SIZE>>(this->pidOrProccessNameVariant))
        {
            const auto &arr = std::get<std::array<char, MESSAGE_SIZE>>(this->pidOrProccessNameVariant);
            std::cout << "Process Name: " << arr.data() << std::endl;
        }
        else
        {
            std::cout << "Variant holds an integer (PID): " << std::get<int>(this->pidOrProccessNameVariant) << std::endl;
        }
    }
    else
    {
        std::cout << "PID: " << std::get<int>(this->pidOrProccessNameVariant) << std::endl;
    }
}

void MessageHeader::setResponse(int iSocket)
{
    this->selfInfo = APPTYPE_SERVER;
    this->response.msgType = MSG_TYPE_RESPONSE;
    this->response.socket = iSocket;
    this->response.sequenceNum = 1;
    string msg = "Response from server " + to_string(this->response.sequenceNum);
    strcpy(this->response.msg, msg.c_str());
}

void MessageHeader::printResponse()
{
    cout << "this->response.msgType : " << this->response.msgType << endl;
    if (this->response.msgType == msgType_e::MSG_TYPE_RESPONSE)
    {
        cout << "sequenceNum : " << this->response.sequenceNum << endl;
        cout << "mess : " << this->response.msg << endl;
    }
}

// Helper function to process the identifier
void MessageHeader::processIdentifier(const string &identifier)
{
    // Consider it's a PID only if:
    // 1. String contains only digits
    // 2. String is not empty
    // 3. String doesn't start with '0' (unless it's just "0")
    // 4. String length is reasonable for a PID (e.g., < 8 digits)
    bool isPid = !identifier.empty() &&
                 identifier.length() < 8 &&
                 (identifier == "0" || identifier[0] != '0') &&
                 std::all_of(identifier.begin(), identifier.end(), ::isdigit);

    this->setIsPis(isPid);

    if (isPid)
    {
        this->setpidOrProccessName(stoi(identifier), "");
    }
    else
    {
        this->setpidOrProccessName(-1, identifier);
    }
}

bool MessageHeader::parseArgumentAndPrepareCommand(const std::vector<string> &args)
{
    bool returnStatus = false;
    int argSize = args.size();
    // Check minimum arguments

    this->setSelfInfo(appType_e::APPTYPE_CLIENT);
    this->setMsgType(msgType_e::MSG_TYPE_CMD);

    if (args[0] == "get-pid")
    {
        // Set message type as CMD
        this->setCommand(command_e::CMD_GET_PID);
        returnStatus = true;
    }
    else if (args[0] == "get-meminfo" && !(argSize < 2))
    {
        this->setCommand(command_e::CMD_GET_MEMORY);
        this->processIdentifier(args[1]);
        returnStatus = true;
    }
    else if (args[0] == "get-cpuinfo" && !(argSize < 2))
    {
        this->setCommand(command_e::CMD_GET_CPU_USAGE);
        this->processIdentifier(args[1]);
        returnStatus = true;
    }
    else if (args[0] == "get-portinfo" && !(argSize < 2))
    {
        this->setCommand(command_e::CMD_GET_PORT_USED);
        this->processIdentifier(args[1]);
        returnStatus = true;
    }
    else if (args[0] == "kill" && !(argSize < 2))
    {
        this->setCommand(command_e::CMD_KILL_PROCESS);
        this->processIdentifier(args[1]);
        returnStatus = true;
    }
    else
    {
        cout << "Invalid command, please provide command such as get [pid, meminfo, cpuinfo, kill] along with either pid or processname\n";
    }

    return returnStatus;
}

MessageHeader::~MessageHeader()
{
}
