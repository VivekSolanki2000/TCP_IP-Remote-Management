#include "RemoteManagement.hh"
#include "History.hh"
#include "NetworkSettings.hh"
#include "MessageHandle.hh"

// Global history object
History commandHistory;
int history_index = -1;
deque<MessageHeader> responseDeque;

string read_input()
{
    struct termios oldt, newt;
    string input;
    int cursor_pos = 0;

    // Save current terminal settings
    if (tcgetattr(STDIN_FILENO, &oldt) != 0)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Modify terminal settings for character-by-character input
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) != 0)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) != 1)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (ch == '\n')
        { // End of input
            cout << endl;
            break;
        }
        else if (ch == 127)
        { // Backspace
            if (cursor_pos > 0)
            {
                input.erase(cursor_pos - 1, 1);
                cursor_pos--;
                cout << "\b \b";
                cout.flush();
                cout << input.substr(cursor_pos) << " ";
                cout.flush();
                cout << "\b";
                cout.flush();
                for (int i = cursor_pos; i < (int)input.size(); i++)
                {
                    cout << "\b";
                    cout.flush();
                }
            }
        }
        else if (ch == 27) // ESC before Arrow key
        {                  // Arrow keys
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) != 1 || read(STDIN_FILENO, &seq[1], 1) != 1)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (seq[0] == '[')
            {
                if (seq[1] == 'A')
                { // Up arrow - previous command
                    if (history_index > 0)
                    {
                        history_index--;
                        refreshLine(cursor_pos);
                        input = commandHistory.getCommand(history_index);
                        cursor_pos = input.length();
                        cout << "\r" << CMDPROMPT << input;
                        cout.flush();
                    }
                }
                else if (seq[1] == 'B')
                { // Down arrow - next command
                    if (history_index < commandHistory.size() - 1)
                    {
                        history_index++;
                        refreshLine(cursor_pos);
                        input = commandHistory.getCommand(history_index);
                        cursor_pos = input.length();
                        cout << "\r" << CMDPROMPT << input;
                        cout.flush();
                    }
                    else
                    {
                        history_index = commandHistory.size();
                        refreshLine(cursor_pos);
                        input.clear();
                        cursor_pos = 0;
                        cout << "\r" << CMDPROMPT;
                        cout.flush();
                    }
                }
                else if (seq[1] == 'C')
                { // Right arrow
                    if (cursor_pos < (int)input.size())
                    {
                        cout << input[cursor_pos];
                        cout.flush();
                        cursor_pos++;
                    }
                }
                else if (seq[1] == 'D')
                { // Left arrow
                    if (cursor_pos > 0)
                    {
                        cout << "\b";
                        cout.flush();
                        cursor_pos--;
                    }
                }
            }
        }
        else
        {
            // Regular character input
            if (cursor_pos < (int)input.size())
            {
                input.insert(cursor_pos, 1, ch);
            }
            else
            {
                input += ch;
            }
            cursor_pos++;
            cout << "\r" << CMDPROMPT << input;
            cout.flush();

            for (int i = cursor_pos; i < (int)input.size(); i++)
            {
                cout << "\b";
                cout.flush();
            }
        }
    }

    // Restore original terminal settings
    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) != 0)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    return input;
}

/**
 * @brief Clears the current line by moving the cursor back and erasing characters.
 *
 *
 * @param cursor_pos The number of characters to move back and erase.
 */
void refreshLine(int cursor_pos)
{
    for (int i = cursor_pos; i; i--)
    {
        cout << "\b \b";
        cout.flush();
    }
}

/**
 * @brief Adds a new command to the history.
 *
 * @param command The command string to add to history.
 */
void add_to_history(const string &command)
{
    if (commandHistory.size() < HISTORY_SIZE)
    {
        commandHistory.addCommand(command);
    }
    else
    {
        commandHistory.eraseFirst();
        commandHistory.addCommand(command);
    }
    history_index = commandHistory.size();
}

/**
 * @brief Parses the input string into a vector of arguments.
 *
 * This function splits the input string into separate arguments,
 * handling quoted strings appropriately.
 *
 * @param input The input string to parse.
 * @return vector<string> A vector of parsed arguments.
 */
vector<string> parse_input(const string &input)
{

    vector<string> args;
    string current_arg;
    bool in_quote = false;
    char quote_char = '\0';

    for (char c : input)
    {
        if (!in_quote && (c == '"' || c == '\''))
        {
            in_quote = true;
            quote_char = c;
        }
        else if (in_quote && c == quote_char)
        {
            in_quote = false;
            quote_char = '\0';
        }
        else if (!in_quote && isspace(c))
        {
            if (!current_arg.empty())
            {
                args.push_back(expandArguments(current_arg));
                current_arg.clear();
            }
        }
        else
        {
            if (quote_char)
            {
                cout << "Error: Unclosed quote detected." << endl;
                args.clear();
                return args;
            }
            current_arg += c;
        }
    }

    if (!current_arg.empty())
    {
        args.push_back(expandArguments(current_arg));
    }

    if (args.size() >= MAX_INPUT_SIZE)
    {
        cerr << "Error: Too many arguments\n";
    }

    return args;
}

string expandArguments(const string &arg)
{
    string expanded_arg;
    size_t pos = 0;
    while (pos < arg.length())
    {
        expanded_arg += arg[pos];
        ++pos;
    }
    return expanded_arg;
}

void exitFun()
{
    commandHistory.saveHistory();
}

void signal_handler(int signo)
{
    switch (signo)
    {
    case SIGINT:
    case SIGTERM:
    case SIGTSTP:
    default:
        cout << "\n"
             << CMDPROMPT;
        cout.flush();
        break;
    }
}

void sendResponse()
{
    cout << "sendResponse " << endl;
    while (true)
    {
        if (!responseDeque.empty())
        {
            MessageHeader responseToBeSent = responseDeque.front();
            responseDeque.pop_front();

            responseToBeSent.printResponse();

            send(responseToBeSent.getSocketIdToSendResponse(), &responseToBeSent, sizeof(responseToBeSent), 0);
        }
    }
}

void receiveResponse(int iSocketId)
{
    MessageHeader incomingMessage;

    cout << "receive Response Th" << endl;
    size_t received_size = 0;
    while (true)
    {
        received_size = recv(iSocketId, &incomingMessage, sizeof(incomingMessage), 0);
        // If recv returns 0, client disconnected

        cout << "received_size =" << received_size << endl;
        // If the full message is received (based on expected message size)
        if (received_size == sizeof(incomingMessage))
        {
            cout << " about to print response " << endl;
            // Process the received message
            incomingMessage.printResponse();
        }
    }
}
