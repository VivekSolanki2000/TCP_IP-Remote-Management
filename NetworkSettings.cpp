#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "NetworkSettings.hh"

/* In Constructor initialise variables of class */
NetworkSettings::NetworkSettings() : sock(0)
{
}

/*********************************************************************
 * @fn      		  - handleClient
 * @brief             - This function handles the receiving data from the client and
 *                      sends a response
 * @param[in]         - int clientSocket
 * @return            - None
 * @Note              -
 *********************************************************************/
void NetworkSettings::handleClient(int clientSocket)
{
    char buffer[BUFFER_SIZE] = {0};
    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int valRead = read(clientSocket, buffer, BUFFER_SIZE);

        if (valRead <= 0)
            break;

        cout << "Message from client " << clientSocket << ": " << buffer << endl;

        // Send client a response
        const char *response = "Message received";
        send(clientSocket, response, strlen(response), 0);
    }
    close(clientSocket);
}

/*********************************************************************
 * @fn      		  - initializeAsServer
 * @brief             - This function initialise the application in
 *                      server mode with configured parameter and waits
 *                      for client to connect on 8080 port
 * @param[in]         - none
 * @return            - bool
 * @Note              -
 *********************************************************************/
bool NetworkSettings::initializeAsServer()
{
    int opt = 1;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        cerr << "Socket creation failed" << endl;
        return false;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        cerr << "Setsockopt failed" << endl;
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        cerr << "Bind failed" << endl;
        return false;
    }

    if (listen(sock, 10) < 0)
    {
        cerr << "Listen failed" << endl;
        return false;
    }

    cout << "Server listening on port " << PORT << endl;
    return true;
}

/*********************************************************************
 * @fn      		  - initializeAsServer
 * @brief             - This function accepts all the client conne ction
 *                      and runs a new thread for each of them and stores
 *                      thread details in clientThreads vector
 * @param[in]         - none
 * @return            - none
 * @Note              -
 *********************************************************************/
void NetworkSettings::runServer()
{
    int addrlen = sizeof(address);

    while (true)
    {
        int clientSocket = accept(sock, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (clientSocket < 0)
        {
            cerr << "Accept failed" << endl;
            continue;
        }

        cout << "New client connected: " << clientSocket << endl;
        // NOTE: emplace_back constructs the new element in place using the arguments provided. This avoids the extra copy or move operation required when using push_back.
        clientThreads.emplace_back(&NetworkSettings::handleClient, this, clientSocket);
    }
}

/*********************************************************************
 * @fn      		  - initializeAsClient
 * @brief             - This function initialise the application in
 *                      client mode with configured parameter and
 *                      tries to connect with server based on given serverIP
 * @param[in]         - const char *serverIP
 * @return            - bool
 * @Note              -
 *********************************************************************/
bool NetworkSettings::initializeAsClient(const char *serverIP)
{
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cerr << "Socket creation failed" << endl;
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, serverIP, &address.sin_addr) <= 0)
    {
        cerr << "Invalid address" << endl;
        return false;
    }

    if (connect(sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        cerr << "Connection failed" << endl;
        return false;
    }

    cout << "Connected to server" << endl;
    return true;
}

/*********************************************************************
 * @fn      		  - runClient
 * @brief             - This function handles the communication part on
 *                      client side
 * @param[in]         - none
 * @return            - none
 * @Note              -
 *********************************************************************/
void NetworkSettings::runClient()
{
    char buffer[BUFFER_SIZE] = {0};
    while (true)
    {
        string message;
        cout << CMDPROMPT;
        cout.flush();     
        getline(cin, message);

        if (message == "quit")
            break;

        send(sock, message.c_str(), message.length(), 0);
        memset(buffer, 0, BUFFER_SIZE);
        read(sock, buffer, BUFFER_SIZE);
        cout << "Server: " << buffer << endl;
    }
}

/* In Destructor De-initialise variables of class */
NetworkSettings::~NetworkSettings()
{
    for (auto &thread : clientThreads)
    {
        if (thread.joinable())
            thread.join();
    }
    close(sock);
}