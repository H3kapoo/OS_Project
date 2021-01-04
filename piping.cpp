#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <vector>
#include <sys/wait.h>
#include <bits/stdc++.h>
using namespace std;

/*
-------------------------------------------------------------
-                 SHELL PIPING HANDLER                      -
-------------------------------------------------------------
*/

/*GLOBALS*/
int MAX_COMMANDS_ALLOWED = 10;

/*String splitter*/
vector<string> split(string source, char separator)
{
    vector<string> tokens;

    stringstream check1(source);

    string intermediate;

    while (getline(check1, intermediate, separator))
        if (intermediate.size())
            tokens.push_back(intermediate);

    return tokens;
}

int main(int argc, char **argv)
{
    vector<string> cmds[10];
    string line(argv[1]);
    string redirFile;

    int cmdNo = 0;

    //use argv[2] to specify if we have redirections to deal with
    bool redir = strcmp(argv[2], "redir") == 0 ? true : false;

    //if we have redirections
    if (redir)
    {
        //parse for individual commands
        vector<string> preParsed = split(line, '>');
        vector<string> parsed = split(preParsed[0], '|');

        //populate commands array
        for (int i = 0; i < parsed.size(); i++)
        {
            vector<string> p = split(parsed[i], ' ');
            cmds[i] = p;
            cmdNo++;
        }

        //get the redirFile name ,white space stripped
        for (int i = 0; i < preParsed[1].size(); i++)
            if (preParsed[1][i] != ' ')
                redirFile.push_back(preParsed[1][i]);
    }
    //if we don't have any redirections
    else
    {
        //parse for individual commands
        vector<string> parsed = split(line, '|');

        //populate commands array
        for (int i = 0; i < parsed.size(); i++)
        {
            vector<string> p = split(parsed[i], ' ');
            cmds[i] = p;
            cmdNo++;
        }
    }

    int link[MAX_COMMANDS_ALLOWED][2], flink[2], errlink[2];

    //check for errors when creating the pipes
    if (pipe(flink) == -1 || pipe(errlink) == -1)
    {
        cerr << "Error creating pipes\n";
        return 1;
    }

    for (int i = 0; i < MAX_COMMANDS_ALLOWED; i++)
    {
        if (pipe(link[i]) == -1)
        {
            cerr << "Error creating pipes\n";
            return 1;
        }
    }

    //process each command
    for (int i = 0; i < cmdNo; i++)
    {
        //convert from string type command to execvp char** compatible type
        int x = 0;
        char **pReadyCmd = new char *[10];

        for (int j = 0; j < cmds[i].size(); j++)
        {
            pReadyCmd[x] = (char *)cmds[i][j].c_str();
            x++;
        }
        pReadyCmd[x + 1] = NULL;

        //split process
        int pid = fork();

        //child used to execute the command
        if (pid == 0)
        {
            dup2(errlink[1], STDERR_FILENO); //redirect errors to errlink

            if (i != 0) //if not first command
            {
                dup2(link[i - 1][0], STDIN_FILENO); //get input from pipe i

                //if last cmd
                if (i + 1 == cmdNo)
                    dup2(flink[1], STDOUT_FILENO); //redirect last cmd ouput to flink
                else
                    dup2(link[i][1], STDOUT_FILENO); //redirect output to pipe i
            }
            else //if first command
            {
                if (cmdNo != 1)
                    dup2(link[i][1], STDOUT_FILENO); //redirect output to pipe i
                else
                    dup2(flink[1], STDOUT_FILENO); //redirect output to pipe flink
            }

            //close fds
            close(flink[0]);
            close(flink[1]);
            close(errlink[0]);
            close(errlink[1]);

            for (int i = 0; i < 10; i++)
            {
                close(link[i][0]);
                close(link[i][1]);
            }

            //execute the command
            execvp(pReadyCmd[0], pReadyCmd);

            //if errors
            fprintf(stderr, "Command '%s' not found!\n", pReadyCmd[0]);
            exit(EXIT_FAILURE);
        }
    }

    //close pipes in parent aswell
    for (int i = 0; i < 10; i++)
    {
        close(link[i][0]);
        close(link[i][1]);
    }
    close(errlink[1]);
    close(flink[1]);

    //wait for processes to finish
    while (wait(NULL) != -1)
        ;

    bool isError = false;
    int ind = 0, x = 0;
    char b, buff[1024];

    //read the error link for any data
    //if we find something,we have an error,display it
    while ((x = read(errlink[0], &b, 1)) > 0)
    {
        buff[ind] = b;
        ind++;
        isError = true;
    }
    buff[ind] = '\0';

    if (ind != 0)
        cout << buff;

    //if there's not error,display end result to terminal or write it to a file
    //depending on 'hasRedir' variable
    if (!isError)
    {
        char buff2[4096];
        int r = read(flink[0], &buff2, 4096);
        buff2[r] = '\0';

        //decide whether to write to file or to screen
        if (redir)
        {
            char *name = get_current_dir_name();
            char path[512];

            strcpy(path, name);
            strcat(path, redirFile.c_str());

            FILE *fp = fopen(path, "w");

            if (fp == NULL)
            {
                cerr << "term: Could not open file\n";
                return 1;
            }

            dup2(fileno(fp), STDOUT_FILENO);
        }
        cout << buff2;
    }
    //close remaining fd
    close(flink[0]);

    return 0;
}