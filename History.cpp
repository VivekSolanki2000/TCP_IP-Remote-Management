#include "History.hh"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

HistoryNode::HistoryNode() {}

HistoryNode::HistoryNode(int num, string command) {
    cnum = num;
    timestamp = getCurrentTime();
    cmd = command;
}

HistoryNode::HistoryNode(int num, string tstamp, string command) {
    cnum = num;
    timestamp = tstamp;
    cmd = command;
}

string HistoryNode::getCurrentTime() {
    time_t now = time(nullptr);
    struct tm* localTime = localtime(&now);
    
    stringstream ss;
    ss << put_time(localTime, "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

History::History() : nextCommandNumber(1) {
    historyFilePath = "./.Remotebash_history";
    loadHistory();
}

void History::addCommand(const string& cmd) {
    history.emplace_back(HistoryNode(nextCommandNumber, cmd));
    nextCommandNumber++;
}

string History::getCommand(int index) const {
    if (index >= 0 && index < (int)history.size()) {
        return history[index].cmd;
    }
    return "";
}

int History::size() const {
    return (int)history.size();
}

void History::clear() {
    history.clear();
    nextCommandNumber = 1;
}

void History::eraseFirst() {
    history.erase(history.begin());
}

void History::saveHistory() const {
    ofstream file(historyFilePath);
    if (file.is_open()) {
        for (const auto& node : history) {
            file << node.cnum << " " << node.timestamp << '\t' << node.cmd << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open history file for writing." << endl;
    }
}

void History::loadHistory() {
    ifstream file(historyFilePath);
    if (file.is_open()) {
        history.clear();
        string line;
        
        while (getline(file, line)) {
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

void History::display() const {
    for (const auto& node : history) {
        cout << node.cnum << "  " << node.timestamp << " " << node.cmd << endl;
    }
}

