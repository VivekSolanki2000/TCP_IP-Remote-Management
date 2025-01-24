
#include "NetworkSettings.hh"
#include "RemoteManagement.hh"

int main(int argc, char *argv[])
{
    // To start application must receive the parameter to define behaviour of application as it can act as either server or client.
    /*
     *  To run application as server : ./tcpapp -s
     *  To run application as client : ./tcpapp -c ipaddress_of_server
     */
    if (argc < 2 || (argc < 3 && (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "-b") == 0)))
    {
        cerr << "Usage:\n"
             << argv[0] << " -s            (for server)\n"
             << argv[0] << " -c server_ip  (for client) (port)" << endl;
        return 1;
    }

    NetworkSettings network;
    string mode = argv[1];

    if (mode == "-s")
    {
        if (!network.initializeAsServer())
            return 1;

        network.runServer();
    }
    else if (mode == "-c" && argc == 4)
    {
        if (!network.initializeAsClient(argv[2]))
            return 1;

        network.runClient();
    }
    else
    {
        cerr << "Invalid arguments" << endl;
        return 1;
    }

    return 0;
}