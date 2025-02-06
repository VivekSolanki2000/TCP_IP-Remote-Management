#include "History.hh"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

/* In Constructor initialise variables of class */
HistoryNode::HistoryNode() {}

/*********************************************************************
 * @fn      		  - HistoryNode() [parameterised constructor]
 * @brief             - This constructor  initialise the object of the class
 *                      with provided value in arguments
 * @param[in]         - int num, string command
 * @return            - none
 * @Note              - Constructor Overloading (by different number of arguments)
 *********************************************************************/
HistoryNode::HistoryNode(int num, string command)
{
    cnum = num;
    timestamp = getCurrentTime();
    cmd = command;
}

/*********************************************************************
 * @fn      		  - HistoryNode() [parameterised constructor]
 * @brief             - This constructor  initialise the object of the class
 *                      with provided value in arguments
 * @param[in]         - int num, string tstamp, string command
 * @return            - none
 * @Note              - Constructor Overloading (by different number of arguments)
 *********************************************************************/
HistoryNode::HistoryNode(int num, string tstamp, string command)
{
    cnum = num;
    timestamp = tstamp;
    cmd = command;
}

/*********************************************************************
 * @fn      		  - getCurrentTime() 
 * @brief             - This function is used to get current localtime of the system
 * @param[in]         - none
 * @return            - string
 * @Note              - 
 *********************************************************************/
string HistoryNode::getCurrentTime()
{
    time_t now = time(nullptr);
    struct tm *localTime = localtime(&now);

    stringstream ss;
    ss << put_time(localTime, "%Y-%m-%d %H:%M:%S");

    return ss.str();
}

/*********************************************************************
 * @fn      		  - nextCommandNumber() 
 * @brief             - This function is used to read the previously executed commands 
 *                      from history file
 * @param[in]         - 
 * @return            - History
 * @Note              - 
 *********************************************************************/
History::History() : nextCommandNumber(1)
{
    historyFilePath = "../.Remotebash_history";
    loadHistory();
}

/*********************************************************************
 * @fn      		  - addCommand() 
 * @brief             - This function is used to add latest command entered by 
 *                      user into history file
 * @param[in]         - const string &cmd)
 * @return            - none
 * @Note              - 
 *********************************************************************/
void History::addCommand(const string &cmd)
{
    history.emplace_back(HistoryNode(nextCommandNumber, cmd));
    nextCommandNumber++;
}

/*********************************************************************
 * @fn      		  - getCommand() 
 * @brief             - This function is used get command from the hostory using 
 *                      index
 * @param[in]         - const string &cmd)
 * @return            - string
 * @Note              - This function is used when user presses either left or right arrow key
 *********************************************************************/
string History::getCommand(int index) const
{
    if (index >= 0 && index < (int)history.size())
    {
        return history[index].cmd;
    }
    return "";
}

/*********************************************************************
 * @fn      		  - size() const
 * @brief             - This function is used get command from the hostory using 
 *                      index
 * @param[in]         - none
 * @return            - int
 * @Note              - const behind function name indicates the value returned by this function is constant
 *********************************************************************/
int History::size() const
{
    return (int)history.size();
}

/*********************************************************************
 * @fn      		  - clear()
 * @brief             - This function clears the vector of the history 
 *                      index
 * @param[in]         - none
 * @return            - int
 * @Note              - 
 *********************************************************************/
void History::clear()
{
    history.clear();
    nextCommandNumber = 1;
}

/*********************************************************************
 * @fn      		  - eraseFirst()
 * @brief             - This function clears the data from the front of the vector of the history 
 * @param[in]         - none
 * @return            - nonw
 * @Note              - 
 *********************************************************************/
void History::eraseFirst()
{
    history.erase(history.begin());
}

/*********************************************************************
 * @fn      		  - saveHistory() const
 * @brief             - This function is used write all latest commands entered by the
 *                      user into history file
 * @param[in]         - none
 * @return            - none
 * @Note              - const behind function name indicates the value returned by this function is constant
 *********************************************************************/
void History::saveHistory() const
{
    ofstream file(historyFilePath);
    if (file.is_open())
    {
        for (const auto &node : history)
        {
            file << node.cnum << " " << node.timestamp << '\t' << node.cmd << endl;
        }
        file.close();
    }
    else
    {
        cerr << "Unable to open history file for writing." << endl;
    }
}

/*********************************************************************
 * @fn      		  - loadHistory() 
 * @brief             - This function is used read all latest commands saved in
 *                      history file
 * @param[in]         - none
 * @return            - none
 * @Note              - 
 *********************************************************************/
void History::loadHistory()
{
    ifstream file(historyFilePath);
    if (file.is_open())
    {
        history.clear();
        string line;

        while (getline(file, line))
        {
            stringstream ss(line);
            int cnum;
            string timestamp;
            string cmd;

            ss >> cnum;
            ss.ignore();
            getline(ss, timestamp, '\t');
            getline(ss, cmd);

            history.emplace_back(HistoryNode(cnum, timestamp, cmd));
            nextCommandNumber = max(nextCommandNumber, cnum + 1);
        }
        file.close();
    }
}

/*********************************************************************
 * @fn      		  - display() const
 * @brief             - This function is used display commands saved in
 *                      history file
 * @param[in]         - none
 * @return            - none
 * @Note              - 
 *********************************************************************/
void History::display() const
{
    for (const auto &node : history)
    {
        cout << node.cnum << "  " << node.timestamp << " " << node.cmd << endl;
    }
}
