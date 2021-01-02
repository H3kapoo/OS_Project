#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>
using namespace std;

/*
-------------------------------------------------------------
-            SHELL SELFMADE 'ENV' COMMAND                   -
-------------------------------------------------------------
*/

//EXTERNS
extern char **environ;

//Simulates 'env' linux command with options -u
int main(int argc, char **argv)
{

    int ch;

    opterr = 0;

    //parse options given
    while ((ch = getopt(argc, argv, "u:")) != -1)
        switch (ch)
        {
        case 'u':
            if (unsetenv(optarg) != 0)
            {
                fprintf(stderr, "Error at removing env variable '%s'! Exiting..\n", optarg);
                return 1;
            }
            break;
        case '?':
            if (optopt == 'u')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default:
            abort();
        }

    //print env variables
    for (char **env = environ; *env != 0; env++)
    {
        char *thisEnv = *env;
        cout << thisEnv << endl;
    }
    return 0;
}