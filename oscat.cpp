#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ioctl.h>
using namespace std;

/*
-------------------------------------------------------------
-            SHELL SELFMADE 'CAT' COMMAND                   -
-------------------------------------------------------------
*/

//GLOBALS
int lineCounter = 1;

/*Check if it's not a directory*/
bool isFile(const string &fileName)
{
    //check if it's actually a file & not a dir
    DIR *dir = opendir(fileName.c_str());

    if (dir == NULL && errno == ENOTDIR)
        return true;
    return false;
}

/*Print file contents based on multiple params*/
void printFileContents(const string &fileName, bool suppress, bool displayDollar, bool numberAllLines, bool numberNonEmptyOnly)
{
    ifstream infile(fileName);
    string line;
    int prevLength = 1;
    bool ok = true;

    //parse line by line from stream
    while (getline(infile, line))
    {
        if (suppress)
        {
            if (line.size() == 0 && ok)
            {
                if (numberAllLines && !numberNonEmptyOnly)
                    printf("%5d  ", lineCounter);
                if (!numberNonEmptyOnly)
                {
                    cout << line << (displayDollar ? "$" : "") << endl;
                    lineCounter++;
                }
                else
                    cout << endl;
                ok = false;
            }
            else if (line.size() != 0)
            {
                ok = true;
                if (numberAllLines || numberNonEmptyOnly)
                    printf("%5d  ", lineCounter);
                cout << line << (displayDollar ? "$" : "") << endl;
                lineCounter++;
            }
        }
        //not suppressed
        else
        {
            if (numberNonEmptyOnly)
            {
                if (line.size() != 0)
                {
                    printf("%5d  ", lineCounter);
                    lineCounter++;
                }
                cout << line << (displayDollar && !numberNonEmptyOnly ? "$" : "") << endl;
            }
            else if (numberAllLines)
            {
                printf("%5d  ", lineCounter);
                cout << line << (displayDollar ? "$" : "") << endl;
                lineCounter++;
            }
            else
                cout << line << (displayDollar ? "$" : "") << endl;
        }
    }
}

//Simulates 'cat' linux command with options -b -s -E -n
int main(int argc, char **argv)
{

    bool s = false, b = false, n = false, E = false;
    int index, c;
    string str;

    opterr = 0;

    //behaviour when no arguments are given
    if (argc == 1)
    {
        //echo what the user inputs
        while (true)
        {
            getline(cin, str);
            if (str == "/.")
                return 0;
            cout << str.c_str() << endl;
        }
    }

    //parse options given
    while ((c = getopt(argc, argv, "bsnE")) != -1)
        switch (c)
        {
        case 'b':
            b = true;
            break;
        case 's':
            s = true;
            break;
        case 'n':
            n = true;
            break;
        case 'E':
            E = true;
            break;
        case '?':
            if (isprint(optopt))
                fprintf(stderr, "cat: Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "cat: Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default:
            abort();
        }

    //check if remaining args are valid files
    for (index = optind; index < argc; index++)
    {
        if (!isFile(argv[index]))
        {
            fprintf(stderr, "cat: %s is not a file or doesn't exist! Exiting..\n", argv[index]);
            return 1;
        }
    }

    //for each file found,print it's contents based on options
    for (index = optind; index < argc; index++)
        printFileContents(argv[index], s, E, n, b);

    return 0;
}