#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <netdb.h>
using namespace std;

/*
-------------------------------------------------------------
-              SHELL SELFMADE 'CLIENT' COMMAND              -
-------------------------------------------------------------
*/

/*Simulate a client connecting to a server*/
int main(int argc, char **argv)
{
    char *IP;
    int port;

    //validate syntax
    if (argc == 3)
    {
        IP = argv[1];
        port = atoi(argv[2]);
    }
    else
    {
        cerr << "Error: Incorect format. Correct format <exec> <ip> <port>\n";
        return 1;
    }

    int socketToServer;

    //create socket
    if ((socketToServer = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cerr << "Networking Error: Failed to create socket\n";
        return 1;
    }

    //set-up server details
    struct sockaddr_in serverDetails;
    serverDetails.sin_addr.s_addr = inet_addr(IP);
    serverDetails.sin_family = AF_INET;
    serverDetails.sin_port = htons(port);

    //check if valid ip
    if (inet_pton(AF_INET, IP, &(serverDetails.sin_addr)) == 0)
    {
        cerr << "Networking Error: Invalid IP\n";
        return 1;
    }

    //connect to that server
    if (connect(socketToServer, (struct sockaddr *)&serverDetails, sizeof(serverDetails)) < 0)
    {
        cerr << "Networking Error: Failed to establish a connection\n";
        close(socketToServer);
        return 1;
    }

    string cmd;

    //prompt user for input
    cout << "\033[44mServer " << IP << ":\033[0m ";
    getline(cin, cmd);

    //try to send data to server
    if (send(socketToServer, cmd.c_str(), strlen(cmd.c_str()), 0) < 0)
        cerr << "Networking Error: Failed to send\n";

    char buff[512];
    int r = 0;

    //read and display server response
    while ((r = recv(socketToServer, buff, 512, 0)) > 0)
    {
        buff[r] = '\0';
        cout << buff;
    }

    //close connection to server
    close(socketToServer);

    return 0;
}