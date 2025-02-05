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
    string timestamp;
    string cmd;

    HistoryNode();
    HistoryNode(int num, string command);
    HistoryNode(int num, string tstamp, string command);

private:
    string getCurrentTime();
};

class History {
private:
    vector<HistoryNode> history;
    int nextCommandNumber;
    string historyFilePath;

public:
    History();
    void addCommand(const string& cmd);
    string getCommand(int index) const;
    int size() const;
    void clear();
    void eraseFirst();
    void saveHistory() const;
    void loadHistory();
    void display() const;
};

#endif