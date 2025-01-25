#ifndef REMOTE_MANAGEMENT_H
#define REMOTE_MANAGEMENT_H

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <termios.h>
#include <vector>

using namespace std;
class NetworkSettings;

void refreshLine(int cursor_pos);
string read_input();
string expandArguments(const string& arg);
vector<string> parse_input(const string& input);
void add_to_history(const string& command);
void exitFun();
#endif
