#include "MessageHandle.hh"

static string cmdStr[CMD_MAX] = 
{
    CMD_GENERATOR(STRING)
};

static string msgStr[MSG_TYPE_MAX] = 
{
    MSG_GENERATOR(STRING)
};

/* In Constructor initialise variables of class with default values*/
MessageHeader::MessageHeader()
{
    selfInfo = APPTYPE_MAX;
    messageSize = 0;
    command = CMD_MAX;
    isPid = 0;
    pidOrProccessNameVariant = 0;
}

/*********************************************************************
 * @fn      		  - MessageHeartBeat()
 * @brief             - This function used to set value for object used to 
 *                      send hearbeat message to server
 * @param[in]         - appType_e app
 * @return            - None
 * @Note              -
 *********************************************************************/
void MessageHeader::MessageHeartBeat(appType_e app)
{
    this->setSelfInfo(app);
    this->setCommand(command_e::CMD_MAX);
    this->setMsgType(msgType_e::MSG_HEARTBEAT);
}

/*********************************************************************
 * @fn      		  - checkIsPid()
 * @brief             - This function used to check the calue of isPid flag 
 *                      on server side to parse the value of varient acordingly
 * @param[in]         - none
 * @return            - bool
 * @Note              -
 *********************************************************************/
bool MessageHeader::checkIsPid()
{
    return isPid;
}

/*********************************************************************
 * @fn      		  - setSelfInfo()
 * @brief             - This function used to validate and set value of 
 *                      selfInfo in the object
 * @param[in]         - appType_e iSelfInfo
 * @return            - none
 * @Note              -
 *********************************************************************/
void MessageHeader::setSelfInfo(appType_e iSelfInfo)
{
    if (iSelfInfo == APPTYPE_SERVER || iSelfInfo == APPTYPE_CLIENT)
        selfInfo = iSelfInfo;
}

/*********************************************************************
 * @fn      		  - setMsgType()
 * @brief             - This function used to validate and set value of 
 *                      msgType in the object
 * @param[in]         - msgType_e iMsgType
 * @return            - none
 * @Note              -
 *********************************************************************/
void MessageHeader::setMsgType(msgType_e iMsgType)
{
    if (iMsgType >= MSG_TYPE_CMD && iMsgType <= MSG_TYPE_MAX)
        msgType = iMsgType;
}

/*********************************************************************
 * @fn      		  - setCommand()
 * @brief             - This function used to validate and set value of 
 *                      command in the object
 * @param[in]         - command_e iCommand
 * @return            - none
 * @Note              -
 *********************************************************************/
void MessageHeader::setCommand(command_e iCommand)
{
    if (iCommand >= CMD_GET_PROCESS && iCommand <= CMD_MAX)
        command = iCommand;
}

/*********************************************************************
 * @fn      		  - setIsPid()
 * @brief             - This function used to  set value of 
 *                      isPid in the object
 * @param[in]         - bool iIsPid
 * @return            - none
 * @Note              -
 *********************************************************************/
void MessageHeader::setIsPid(bool iIsPid)
{
    isPid = iIsPid;
}

/*********************************************************************
 * @fn      		  - setpidOrProccessName()
 * @brief             - This function used to  set value of 
 *                      isPid or process name in the object
 * @param[in]         - int iPid, string iProcessName
 * @return            - none
 * @Note              -
 *********************************************************************/
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

/*********************************************************************
 * @fn      		  - setMessageHandlerInfo()
 * @brief             - This function used to  set value of 
 *                      message in the object
 * @param[in]         - string msg
 * @return            - none
 * @Note              -
 *********************************************************************/
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

/*********************************************************************
 * @fn      		  - printHeader()
 * @brief             - This function used to  print the value of 
 *                      all members in the object
 * @param[in]         - none
 * @return            - none
 * @Note              -
 *********************************************************************/
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

/*********************************************************************
 * @fn      		  - setResponse()
 * @brief             - This function used to set value of MessageHeader and response class
 *                      members in the object
 * @param[in]         - appType_e type, msgType_e mType,int clientSocket, int sequenceNum,string resp
 * @return            - none
 * @Note              -
 *********************************************************************/
void MessageHeader::setResponse(appType_e type, msgType_e mType, int clientSocket, int sequenceNum,string resp)
{
    this->selfInfo = type;
    this->msgType = mType;
    this->response.socket = clientSocket;
    this->response.sequenceNum = sequenceNum;
    strcpy(this->response.msg, resp.c_str());
}

/*********************************************************************
 * @fn      		  - printResponse()
 * @brief             - This function used to print the value of response class
 *                      members in the object
 * @param[in]         - none
 * @return            - none
 * @Note              -
 *********************************************************************/
void MessageHeader::printResponse()
{
    if (this->msgType == msgType_e::MSG_TYPE_RESPONSE)
    {
        cout << this->response.msg;
        cout.flush();
    }
}

/*********************************************************************
 * @fn      		  - processIdentifier()
 * @brief             - This function used to identify the detail of process name
 *                      whether that is process name or if process id and set the value of 
 *                      variant accordingly
 * @param[in]         - const string &identifier
 * @return            - none
 * @Note              -
 *********************************************************************/
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

    this->setIsPid(isPid);

    if (isPid)
    {
        this->setpidOrProccessName(stoi(identifier), "");
    }
    else
    {
        this->setpidOrProccessName(-1, identifier);
    }
}

/*********************************************************************
 * @fn      		  - parseArgumentAndPrepareCommand()
 * @brief             - This function used to parse the argument entered by the user
 *                      and prepare message accordingly to sent to server
 * @param[in]         - const vector<string> &args
 * @return            - bool
 * @Note              -
 *********************************************************************/
bool MessageHeader::parseArgumentAndPrepareCommand(const vector<string> &args)
{
    bool returnStatus = false;
    int argSize = args.size();
    // Check minimum arguments

    this->setSelfInfo(appType_e::APPTYPE_CLIENT);
    this->setMsgType(msgType_e::MSG_TYPE_CMD);

    if (args[0] == cmdStr[CMD_GET_PROCESS])
    {
        // Set message type as CMD
        this->setCommand(command_e::CMD_GET_PROCESS);
        returnStatus = true;
    }
    else if (args[0] == cmdStr[CMD_GET_MEMORY] && !(argSize < 2))
    {
        this->setCommand(command_e::CMD_GET_MEMORY);
        this->processIdentifier(args[1]);
        returnStatus = true;
    }
    else if (args[0] == cmdStr[CMD_GET_CPU_USAGE] && !(argSize < 2))
    {
        this->setCommand(command_e::CMD_GET_CPU_USAGE);
        this->processIdentifier(args[1]);
        returnStatus = true;
    }
    else if (args[0] == cmdStr[CMD_GET_PORT_USED] && !(argSize < 2))
    {
        this->setCommand(command_e::CMD_GET_PORT_USED);
        this->processIdentifier(args[1]);
        returnStatus = true;
    }
    else if (args[0] == cmdStr[CMD_KILL_PROCESS] && !(argSize < 2))
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


/*********************************************************************
 * @fn      		  - getProcessName()
 * @brief             - This function used to get the process name entered by the user
 *                      to perform operations
 * @param[in]         - none
 * @return            - string
 * @Note              -
 *********************************************************************/
string MessageHeader::getProcessName()
{
    const auto& charArray = get<array<char, MESSAGE_SIZE>>(this->pidOrProccessNameVariant);
    return string(charArray.data());
}

/*********************************************************************
 * @fn      		  - getProcessId()
 * @brief             - This function used to get the process Id entered by the user
 *                      to perform operations
 * @param[in]         - none
 * @return            - int
 * @Note              -
 *********************************************************************/
int MessageHeader::getProcessId()
{
    return get<int>(this->pidOrProccessNameVariant);
}

/* In Destructor De-initialise variables of class */
MessageHeader::~MessageHeader()
{
}
