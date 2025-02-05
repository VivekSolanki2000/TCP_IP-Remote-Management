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

#define		ENUM(ENUM, STRING)		ENUM,
#define		STRING(ENUM, STRING)	STRING,

#define APPTYPE_GENERATOR(ARG)              \
    ARG(APPTYPE_SERVER,"APPTYPE_SERVER")    \
    ARG(APPTYPE_CLIENT,"APPTYPE_CLIENT")    

typedef enum
{
    APPTYPE_GENERATOR(ENUM)
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
void sendRequest(int iSocketId);
void receiveResponse(int iSocketId);

#endif
