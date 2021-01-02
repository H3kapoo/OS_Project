#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

/*
-------------------------------------------------------------
-              SHELL SELFMADE 'SERVER' PROGRAM              -
-------------------------------------------------------------
*/

/*Thread function to handle each connection*/
void *handleConnection(void *arg)
{

    int pid;
    int newSocketFd = *(int *)arg;
    char recvCmd[128];

    //receive command from client
    int r = recv(newSocketFd, recvCmd, 128, 0);
    recvCmd[r] = '\0';
    cout << "Client sent: " << recvCmd << endl;

    //create new process to execute that command and send it back to the client
    if ((pid = fork()) == 0)
    {
        dup2(newSocketFd, STDOUT_FILENO);
        dup2(newSocketFd, STDERR_FILENO);

        char **args = new char *[3];
        args[0] = (char *)"shell";
        args[1] = recvCmd;
        args[2] = NULL;

        char sr[100] = "\033[44m >> Server Returned <<\033[0m \n";
        send(newSocketFd, sr, strlen(sr), 0);

        execv(args[0], args);
        fprintf(stderr, "Command '%s' not found!\n", args[0]);
        exit(EXIT_FAILURE);
    }
    waitpid(pid, NULL, 0);

    char sr[100] = "\033[44m >> Transmision End<<\033[0m \n";
    send(newSocketFd, sr, strlen(sr), 0);

    //close connection to client
    close(newSocketFd);

    return NULL;
}
//Simulates a server running on a specific port
int main(int argc, char **argv)
{

    int socketFd, numberConnected = 0, on = 1;
    int PORT = 5021;

    //create a socket for the server
    if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cerr << "Networking Error: Failed to create socket\n";
        return 1;
    }

    //make port reusable
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        cerr << "Networking Error: Setopt error\n";
        return 1;
    }

    //set up server details
    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    //bind server to socket
    if (bind(socketFd, (sockaddr *)&server, sizeof(server)) < 0)
    {
        cerr << "Networking Error: Failed to bind\n";
        close(socketFd);
        return 1;
    }
    //lister to socket
    if (listen(socketFd, 1) < 0)
    {
        cerr << "Networking Error: Failed to listen\n";
        close(socketFd);
        return 1;
    }

    int newSocketFd;
    int sc = sizeof(struct sockaddr_in);
    cout << "Connected: " << numberConnected << " Waiting for connections.." << endl;

    //wait and accept connections forever
    while (newSocketFd = accept(socketFd, (struct sockaddr *)&client, (socklen_t *)&sc))
    {
        numberConnected++;
        cout << "Connected: " << numberConnected << " Waiting for connections.." << endl;

        //create thread for each new connection
        pthread_t t;
        pthread_create(&t, NULL, &handleConnection, &newSocketFd);
    }

    //close server socket
    close(socketFd);

    return 0;
}