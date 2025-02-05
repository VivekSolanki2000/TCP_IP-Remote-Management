#ifndef HISTORY_H
#define HISTORY_H

#include <string>
#include <vector>
#include <iostream>
#include "RemoteManagement.hh"

#define MAX_INPUT_SIZE 1000
#define HISTORY_SIZE 100


class HistoryNode {
public:
    int cnum;
    std::string timestamp;
    std::string cmd;

    HistoryNode();
    HistoryNode(int num, std::string command);
    HistoryNode(int num, std::string tstamp, std::string command);

private:
    std::string getCurrentTime();
};

class History {
private:
    std::vector<HistoryNode> history;
    int nextCommandNumber;
    std::string historyFilePath;

public:
    History();
    void addCommand(const std::string& cmd);
    std::string getCommand(int index) const;
    int size() const;
    void clear();
    void eraseFirst();
    void saveHistory() const;
    void loadHistory();
    void display() const;
};

#endif