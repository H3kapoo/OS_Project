#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <vector>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <readline/readline.h>
#include <readline/history.h>
using namespace std;

/*
-----------------------------------------------------------------------
-                    SHELL LAUNCHER PROGRAM                           -
-----------------------------------------------------------------------
*/

/*GLOBALS*/
bool hasRedirOrPipes;
bool hasRedir;

/*String splitter*/
vector<string> split(string source, char separator)
{
    string intermediate;
    stringstream check1(source);
    vector<string> tokens;

    while (getline(check1, intermediate, separator))
        if (intermediate.size())
            tokens.push_back(intermediate);

    return tokens;
}

/*Print help command*/
void printHelp()
{
    cout << "------Available commands list------" << endl;
    cout << " 1.cat     with options -b -E -n -s" << endl;
    cout << " 2.head    with options -c -n -q -v" << endl;
    cout << " 3.env     with options -u" << endl;
    cout << "-----------------------------------" << endl;
}

/*Print version command*/
void printVersion()
{
    cout << "---------Shell information---------" << endl;
    cout << "Author:  Andrei Boje" << endl;
    cout << "Version: 1.0" << endl;
    cout << "PS:      Keanu Reeves was here" << endl;
    cout << "-----------------------------------" << endl;
}

/*Executed piping.cpp for the given 'line' */
void processPipesAndRedir(string line)
{
    char **piping = new char *[4];

    piping[0] = (char *)"piping";
    piping[1] = (char *)line.c_str();

    if (hasRedir)
        piping[2] = (char *)"redir";
    else
        piping[2] = (char *)"noredir";
    piping[3] = NULL;

    int pid = fork();
    if (pid == 0)
        execv("piping", piping);
    waitpid(pid, NULL, 0);
}

/*Convert from string to char** and exec*/
void execShorthand(vector<string> source, char *cmd, bool isLinuxOwnCommand)
{

    int n = source.size();
    const char **args = new const char *[20];

    args[0] = cmd;

    for (int i = 1; i < n; i++)
        args[i] = source[i].c_str();

    args[n + 1] = NULL;

    if (isLinuxOwnCommand)
        execvp(args[0], (char **)args);
    else
        execv(args[0], (char **)args);

    printf("Command '%s' doesn't exist!\n", args[0]);
}

/*Validate user input syntax*/
bool isValidLine(string line)
{
    //if line is empty,is invalid
    if (line.size() == 0)
        return false;

    bool ok = true;
    int redirs = 0;
    int endIndex;
    string supr;
    string symb;

    //suppress command of spaces
    for (int i = 0; i < line.size(); i++)
        if (line[i] != ' ')
            supr.push_back(line[i]);

    endIndex = supr.size();

    for (int i = 0; i < supr.size(); i++)
        if (supr[i] == '|' || supr[i] == '>')
            symb.push_back(supr[i]);

    //start of end unexpected | >
    if (supr[0] == '|' || supr[supr.size() - 1] == '|')
    {
        cout << "term: syntax error near unexpected token '|'\n";
        return false;
    }
    if (supr[0] == '>' || supr[supr.size() - 1] == '>')
    {
        cout << "term: syntax error near unexpected token '>'\n";
        return false;
    }

    //if we have more than one symbol
    if (symb.size() > 1)
    {
        //consecutive unexpected | >
        for (int i = 0; i < supr.size() - 1; i++)
        {
            if ((supr[i] == '|' && supr[i + 1] == '|') || (supr[i] == '>' && supr[i + 1] == '|'))
            {
                cout << "term: syntax error near unexpected token '|'\n";
                return false;
            }
            if ((supr[i] == '>' && supr[i + 1] == '>') || (supr[i] == '|' && supr[i + 1] == '>'))
            {
                cout << "term: syntax error near unexpected token '>'\n";
                return false;
            }

            if (supr[i] == '>')
                redirs++;

            //check redirections
            if (redirs > 1)
            {
                cout << "term: too many unexpected redirections\n";
                return false;
            }
        }

        //if we have > not at the end of symb stack,return
        for (int i = 0; i < symb.size() - 1; i++)
        {
            if (symb[i] == '>')
            {
                cout << "term: syntax error near unexpected token '>'\n";
                return false;
            }
        }
    }
    //if we have only one redirection or we find > in the symb stack
    if (redirs == 1 || symb.find(">") != string::npos)
        hasRedir = true;

    //check pipes and redirections
    if (symb.size() > 0)
        hasRedirOrPipes = true;

    return true;
}

int main(int argc, char **argv)
{
    //get process pid used for killing it later on
    int mainPid = getpid();

    //check if we are in a networking situation
    bool isNetworking = false;

    if (argv[1] != NULL)
        isNetworking = true;

    //main loop
    while (true)
    {
        hasRedirOrPipes = false;
        hasRedir = false;

        char *input;
        string line;

        //if we are networking,the input comes from argv[1]
        //else we get the command as usual from stdin
        if (isNetworking)
            line = argv[1];
        else
        {
            //compute prompt color line
            char *f = get_current_dir_name();
            f = basename(f);

            char col[] = "\033[42m";
            char col2[] = "\033[46m";
            char cclear[] = "\033[0m";
            char x[128];

            strcpy(x, col);
            strcat(x, f);
            strcat(x, col2);
            strcat(x, " $>");
            strcat(x, cclear);
            strcat(x, " ");

            //get user input
            input = readline(x);
            line = input;

            //add to history
            add_history(input);

            delete input;
        }

        //check if line is syntax valid
        if (isValidLine(line))
        {
            //if line has pipes/redirection, continue with processing them
            if (hasRedirOrPipes)
                processPipesAndRedir(line);
            else
            {
                //parse line in parent and use child to execute the command parsed
                int cmdNo = 0;
                vector<string> parsedInput = split(line, '|');
                vector<string> cmdsBuffer[10];

                for (int i = 0; i < parsedInput.size(); i++)
                {
                    vector<string> parsedInput2 = split(parsedInput[i], ' ');

                    for (int j = 0; j < parsedInput2.size(); j++)
                        cmdsBuffer[cmdNo].push_back(parsedInput2[j]);
                    cmdNo++;
                }

                //check for exit flag
                if (cmdsBuffer[0][0] == "exit")
                    kill(mainPid, SIGKILL);

                int pid = fork();

                //child process executes command
                if (pid == 0)
                {
                    //use self made commands
                    if (cmdsBuffer[0][0] == "cat")
                        execShorthand(cmdsBuffer[0], (char *)"oscat", false);
                    else if (cmdsBuffer[0][0] == "head")
                        execShorthand(cmdsBuffer[0], (char *)"oshead", false);
                    else if (cmdsBuffer[0][0] == "env")
                        execShorthand(cmdsBuffer[0], (char *)"osenv", false);
                    else if (cmdsBuffer[0][0] == "client")
                        execShorthand(cmdsBuffer[0], (char *)"client", false);
                    else if (cmdsBuffer[0][0] == "help")
                        printHelp();
                    else if (cmdsBuffer[0][0] == "version")
                        printVersion();
                    //use linux own commands
                    else
                        execShorthand(cmdsBuffer[0], (char *)cmdsBuffer[0][0].c_str(), true);

                    int s;
                    exit(s);
                }
                waitpid(pid, NULL, 0);
            }
        }

        //if we are networking,run this loop only once
        if (isNetworking)
            break;
    }
    return 0;
}