#include "MessageHandle.hh"

MessageHeader::MessageHeader()
{
    selfInfo = APPTYPE_MAX;
    messageSize = 0;
    command = CMD_MAX;
    isPid = 0;
    pidOrProccessNameVariant = 0;
}

void MessageHeader::MessageHeartBeat(appType_e app)
{
    this->setSelfInfo(app);
    this->setCommand(command_e::CMD_MAX);
    this->setMsgType(msgType_e::MSG_HEARTBEAT);
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
    if (iCommand >= CMD_GET_PROCESS && iCommand <= CMD_MAX)
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
        array<char, MESSAGE_SIZE> charArray{};
        copy_n(iProcessName.begin(), min(iProcessName.length(), static_cast<size_t>(MESSAGE_SIZE - 1)), charArray.data());
        charArray[MESSAGE_SIZE - 1] = '\0';

        this->pidOrProccessNameVariant = charArray;
    }
    else
    {
        pidOrProccessNameVariant = iPid;
    }
}

void MessageHeader::setMessageHandlerInfo(string msg)
{
    this->selfInfo = APPTYPE_CLIENT;
    this->msgType = MSG_TYPE_CMD;
    this->command = CMD_GET_PROCESS;
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

    if (this->isPid == 0 && this->command != CMD_GET_PROCESS)
    {
        if (holds_alternative<array<char, MESSAGE_SIZE>>(this->pidOrProccessNameVariant))
        {
            const auto &arr = get<array<char, MESSAGE_SIZE>>(this->pidOrProccessNameVariant);
            cout << "Process Name: " << arr.data() << endl;
        }
        else
        {
            cout << "Variant holds an integer (PID): " << get<int>(this->pidOrProccessNameVariant) << endl;
        }
    }
    else
    {
        cout << "PID: " << get<int>(this->pidOrProccessNameVariant) << endl;
    }
}

void MessageHeader::setResponse(appType_e type, msgType_e mType, int clientSocket, int sequenceNum,string resp)
{
    this->selfInfo = type;
    this->msgType = mType;
    this->response.socket = clientSocket;
    this->response.sequenceNum = sequenceNum;
    strcpy(this->response.msg, resp.c_str());
}

void MessageHeader::printResponse()
{
    if (this->msgType == msgType_e::MSG_TYPE_RESPONSE)
    {
        cout << this->response.msg;
        cout.flush();
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
                 all_of(identifier.begin(), identifier.end(), ::isdigit);

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

bool MessageHeader::parseArgumentAndPrepareCommand(const vector<string> &args)
{
    bool returnStatus = false;
    int argSize = args.size();
    // Check minimum arguments

    this->setSelfInfo(appType_e::APPTYPE_CLIENT);
    this->setMsgType(msgType_e::MSG_TYPE_CMD);

    if (args[0] == "get-process")
    {
        // Set message type as CMD
        this->setCommand(command_e::CMD_GET_PROCESS);
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
        cout << "Invalid command, please provide command such as get-[process, meminfo, cpuinfo, kill] along with either pid or processname\n";
    }

    return returnStatus;
}


string MessageHeader::getProcessName()
{
    const auto& charArray = get<array<char, MESSAGE_SIZE>>(this->pidOrProccessNameVariant);
    return string(charArray.data());
}

int MessageHeader::getProcessId()
{
    return get<int>(this->pidOrProccessNameVariant);
}

MessageHeader::~MessageHeader()
{
}
