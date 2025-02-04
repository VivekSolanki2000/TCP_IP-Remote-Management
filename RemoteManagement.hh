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
#include <signal.h>
#include <variant>
#include <deque>
#include <array>
#include <algorithm>
#include <mutex>

#define MESSAGE_SIZE 100
#define CMD_SIZE 15
using namespace std;
class NetworkSettings;

typedef enum
{
    APPTYPE_SERVER = 0,
    APPTYPE_CLIENT,
    APPTYPE_MAX,
} appType_e;



void refreshLine(int cursor_pos);
string read_input();
string expandArguments(const string &arg);
vector<string> parse_input(const string &input);
void signal_handler(int signo);
void add_to_history(const string &command);
void exitFun();
void sendResponse();
void receiveResponse(int iSocketId);

#endif
