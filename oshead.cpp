#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <sys/ioctl.h>
using namespace std;

/*
-------------------------------------------------------------
-            SHELL SELFMADE 'HEAD' COMMAND                  -
-------------------------------------------------------------
*/

//Parse char to positive/negative integer
int getNumber(char *opt)
{
    int nr = 0;
    bool hasDash = false;

    while (*opt)
    {
        if ((int)(*opt) == 45 && !hasDash)
            hasDash = true;
        else
        {
            if ((int)(*opt) < 48 || (int)(*opt) > 55)
                return -1;

            nr += (int)(*opt) - 48;
            nr *= 10;
        }
        opt++;
    }
    return nr / 10;
}

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
void printFileContentsHead(const string &fileName, int nLines, int cBytes, bool quiet, bool verbose, bool multipleFiles, bool isFirstFile)
{
    ifstream infile(fileName);
    string line;
    int currLine = 0;
    int currBytes = 0;

    if (nLines == 0)
        nLines = 10;

    if (multipleFiles && !quiet)
        if (isFirstFile)
            cout << "==> " << fileName << " <==" << endl;
        else
            cout << endl
                 << "==> " << fileName << " <==" << endl;
    else if (verbose)
        cout << "==> " << fileName << " <==" << endl;

    while (getline(infile, line))
    {
        if (cBytes != 0 && currBytes < cBytes)
        {
            for (int i = 0; i < line.size() && currBytes < cBytes; i++)
            {
                cout << line[i];
                currBytes += 1;
            }
            cout << endl;
            if (currBytes >= cBytes)
                break;
        }
        else if (nLines != 0 && currLine < nLines)
        {
            cout << line << endl;
            currLine++;

            if (currLine >= nLines)
                break;
        }
    }
}

//Simulates 'head' linux command with options -c -n -q -v
int main(int argc, char **argv)
{

    int c = 0, n = 0, index, ch;
    bool q = false, v = false;
    string str;

    opterr = 0;

    //behaviour when no args are given
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
    while ((ch = getopt(argc, argv, "qvc:n:")) != -1)
        switch (ch)
        {
        case 'c':
            c = getNumber(optarg);
            if (c == -1)
            {
                fprintf(stderr, "head: invalid number of bytes: '%s'\n", optarg);
                return 1;
            }
            break;
        case 'n':
            n = getNumber(optarg);
            if (n == -1)
            {
                fprintf(stderr, "head: invalid number of lines: '%s'\n", optarg);
                return 1;
            }
            break;
        case 'q':
            q = true;
            break;
        case 'v':
            v = true;
            break;
        case '?':
            if (optopt == 'c' || optopt == 'n')
                fprintf(stderr, "head: Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "head: Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "head: Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default:
            abort();
        }

    //check if remaining args are valid files
    for (index = optind; index < argc; index++)
    {
        if (!isFile(argv[index]))
        {
            fprintf(stderr, "head: %s is not a file or doesn't exist! Exiting..\n", argv[index]);
            return 1;
        }
    }

    //check if we have multiple files to deal with
    bool multipleFiles = (argc - optind) > 1 ? true : false;
    int i = 0;

    //for each file found,print it's contents based on options
    for (index = optind; index < argc; index++)
    {
        printFileContentsHead(argv[index], n, c, q, v, multipleFiles, (i == 0) ? true : false);
        i = 1;
    }

    return 0;
}