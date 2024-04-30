#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define SELECTION_SINGLE    (0)
#define SELECTION_ALL       (1)
#define SELECTION_RECURSIVE (2)

#define FORMAT_JSON         (0)
#define FORMAT_TEXT         (1)

struct stat fileInfo;
char * fileName;
int selectionMode;
int humanReadable;
int format;
int logMode;
FILE * logFile;
char * logFileName;
int logReady;

char * logBuffer;

void argsHandler(int argc, char * argv[]);
void help(int argc, char * argv[]);
void printText();
void printJson();
int setupLogFile();
int checkForFlags(char * arg);
void logMsg(char * msg);

int main(int argc, char *argv[]) 
{    
    logReady = 0;
    logBuffer = (char *) malloc(sizeof(char) * 32768);
    logMsg("start of buffer\n");
    
    argsHandler(argc, argv);
    
    if (logMode)
    {
        setupLogFile();
        logMsg(logBuffer);
    }

    if (fileName == NULL)
    {
        fprintf(stderr, "No file dictated for inspecting\n");
        exit(1);
    }
    else if (stat(fileName, &fileInfo) != 0) 
    {
        fprintf(stderr, "Error getting file info for %s: %s\n", fileName, strerror(errno));
        exit(1);
    }
    
    if (format == FORMAT_TEXT)
        printText();
    if (format == FORMAT_JSON)
        printJson();
    
    return 0;
}

void help(int argc, char * argv[])
{

    printf("inspect: inspect [-? | -i <file_path> | -a [directory_path]] [-h]              \n");
    printf("             [-f [text|json]] [-l <log_file_path>]                             \n");
    printf("    -?              Prints this message                                        \n");
    printf("    -i --inode      Dictates the file whose inode information is displayed     \n");
    printf("    -a --all        Dictates that all files in the directory will have their   \n");
    printf("                    inode information printed                                  \n");
    printf("    -r              Subflag of the \"-a\" flag. Recursively prints all files in\n");
    printf("                    all folders of the directory                               \n");
    printf("    -h --human      Prints inode information in a human-readable format        \n");
    printf("    -f --format     Dictates the format of the output                          \n");
    printf("    -f --log        Enables the program to log its execution to the designated \n");
    printf("                    log file                                                   \n");
    
}

void argsHandler(int argc, char * argv[])
{
    if (argc == 1)
    {
        help(argc, argv);
    }
    else if (argc == 2)
    {
        if (checkForFlags(argv[1]))
        {
            if (strcmp(argv[1], "-?") == 0)
            {
                logMsg("    help flag detected\n");
                help(argc, argv); 
                exit(0);
            }
        }
        else
        {
            format = FORMAT_TEXT;
            selectionMode = SELECTION_SINGLE;
            fileName = argv[1];            
        }
    }
    else
    {
        strcat(logBuffer, "  Handling program arguments\n");
        format = FORMAT_TEXT;
        
        for (int i = 1; i < argc; i++)
        {
            int validPostArg = 0;
            
            if (strcmp(argv[i], "-?") == 0)
            {
                logMsg("    help flag detected\n");
                help(argc, argv);
            }
            else if (   strcmp(argv[i], "-i") == 0 ||  
                        strcmp(argv[i], "--inode") == 0)
            {
                logMsg("    inode flag detected\n");
                selectionMode = SELECTION_SINGLE;
                if (i + 1 < argc)
                {
                    if (checkForFlags(argv[i+1]))
                    {
                        logMsg("      detected flag immediately after \"-i\"");
                        printf("Detected flag in fileName position post \"-i\" / \"--inode\"\n");
                    }
                    else
                    {
                        fileName = argv[i+1];
                        validPostArg++;
                    }
                }
                else
                {
                    printf("      no file after inode flag\n");
                }
            } 
            else if (strcmp(argv[i], "-a") == 0)
            {
                logMsg("    all flag detected\n");
             
                selectionMode = SELECTION_ALL;
                if (i + 1 < argc)
                {
                    fileName = argv[i+1];
                    validPostArg++;
                }
                if (i + 2 < argc)
                {
                    if (strcmp(argv[i+2], "-r"))
                    {
                        logMsg("    recursive flag detected\n");
             
                        selectionMode = SELECTION_RECURSIVE;
                        validPostArg++;
                    }
                }
            }
            else if (strcmp(argv[i], "-h") == 0)
            {
                logMsg("    human readable flag detected\n");
                humanReadable = 1;
                
                if (i+1 < argc)
                {
                    if (checkForFlags(argv[i+1]))
                    {
                        logMsg("      invalid post argument");
                        printf("Invalid post argument after \"-h\" / \"-human\" flag\n");
                        printf("There must not be a post argument after flag\n");
                        exit(1);
                    }
                }
            }
            else if (strcmp(argv[i], "-f") == 0)
            {
                logMsg("    format flag detected\n");
                if(i+1 < argc)
                {
                    if (strcmp(argv[i+1], "json") == 0)
                    {
                        logMsg("      json format\n");
                        format = FORMAT_JSON;
                        validPostArg++;
                    }
                    else if (strcmp(argv[i+1], "text") == 0)
                    {
                        logMsg("      text format\n");
                        format = FORMAT_TEXT;
                        validPostArg++;
                    }
                    else
                    {
                        logMsg("      invalid format\n");
                        printf("Invalid format\n");
                        printf("Enter \"text\" or \"json\" after \"-f\" / \"--format\" flag\n");
                        exit(1);
                    }
                }
                else
                {
                    logMsg("      missing format\n");
                    printf("Missing format argument\n");
                    printf("Enter \"text\" or \"json\" after \"-f\" / \"--format\" flag\n");
                    exit(1);
                }
            }
            
            else if (strcmp(argv[i], "-l") == 0)
            {
                
                if (i+1 < argc)
                {
                    logMsg("    log flag detected\n");
                    logFileName = argv[i+1];
                    logMode = 1;
                    validPostArg++;
                }
            }
            i += validPostArg;
        }
    }
}

void printText()
{
    printf("Information for: %s\n", fileName);
    printf("File Inode: %lu\n", fileInfo.st_ino);
    printf("Premission: ");
        if(S_IRUSR&fileInfo.st_mode)    printf("r");    else    printf("-");
        if(S_IWUSR&fileInfo.st_mode)    printf("w");    else    printf("-");
        if(S_IXUSR&fileInfo.st_mode)    printf("x");    else    printf("-");
        if(S_IRGRP&fileInfo.st_mode)    printf("r");    else    printf("-");
        if(S_IRGRP&fileInfo.st_mode)    printf("w");    else    printf("-");
        if(S_IRGRP&fileInfo.st_mode)    printf("x");    else    printf("-");
        if(S_IROTH&fileInfo.st_mode)    printf("r");    else    printf("-");
        if(S_IROTH&fileInfo.st_mode)    printf("w");    else    printf("-");
        if(S_IROTH&fileInfo.st_mode)    printf("x");    else    printf("-");
        printf("\n");
    printf("File Type: ");
        if (S_ISREG(fileInfo.st_mode))          printf("regular file");
        else if (S_ISDIR(fileInfo.st_mode))     printf("directory");
        else if (S_ISCHR(fileInfo.st_mode))     printf("character device");
        else if (S_ISBLK(fileInfo.st_mode))     printf("block device");
        else if (S_ISFIFO(fileInfo.st_mode))    printf("FIFO (named pipe)");
        else if (S_ISLNK(fileInfo.st_mode))     printf("symbolic link");
        else if (S_ISSOCK(fileInfo.st_mode))    printf("socket");
        else                                    printf("unknown?");
        printf("\n");
    printf("Number of Hard Links: %lu\n", fileInfo.st_nlink);
    printf("User ID: %d\n", fileInfo.st_uid);
    printf("Group ID: %d\n", fileInfo.st_gid);
    printf("File Size: %lu bytes\n", fileInfo.st_size);
    printf("Last Access Time: %ld\n", fileInfo.st_atime);
    printf("Last Modification Time: %ld\n", fileInfo.st_mtime);
    printf("Last Status Change Time: %ld\n", fileInfo.st_ctime);
}

void printJson()
{
    printf("{\n");
    printf("\t\"filePath\": \"%s\",\n", fileName);
    printf("\t\"inode\": {\n");
    printf("\t\t\"number\":           %lu,\n", fileInfo.st_ino);
    printf("\t\t\"type\":             \"");
        if (S_ISREG(fileInfo.st_mode))          printf("regular file");
        else if (S_ISDIR(fileInfo.st_mode))     printf("directory");
        else if (S_ISCHR(fileInfo.st_mode))     printf("character device");
        else if (S_ISBLK(fileInfo.st_mode))     printf("block device");
        else if (S_ISFIFO(fileInfo.st_mode))    printf("FIFO (named pipe)");
        else if (S_ISLNK(fileInfo.st_mode))     printf("symbolic link");
        else if (S_ISSOCK(fileInfo.st_mode))    printf("socket");
        else                                    printf("unknown?");
        printf("\",\n");
    printf("\t\t\"permissions\":      \"");
        if(S_IRUSR&fileInfo.st_mode)    printf("r");    else    printf("-");
        if(S_IWUSR&fileInfo.st_mode)    printf("w");    else    printf("-");
        if(S_IXUSR&fileInfo.st_mode)    printf("x");    else    printf("-");
        if(S_IRGRP&fileInfo.st_mode)    printf("r");    else    printf("-");
        if(S_IRGRP&fileInfo.st_mode)    printf("w");    else    printf("-");
        if(S_IRGRP&fileInfo.st_mode)    printf("x");    else    printf("-");
        if(S_IROTH&fileInfo.st_mode)    printf("r");    else    printf("-");
        if(S_IROTH&fileInfo.st_mode)    printf("w");    else    printf("-");
        if(S_IROTH&fileInfo.st_mode)    printf("x");    else    printf("-");
        printf("\",\n");
    printf("\t\t\"linkCount\":        %lu,\n", fileInfo.st_nlink);
    printf("\t\t\"uid\":              %d\n", fileInfo.st_uid);
    printf("\t\t\"gid\":              %d\n", fileInfo.st_gid);
    printf("\t\t\"size\":             %lu,\n", fileInfo.st_size);
    printf("\t\t\"accessTime\":       %ld,\n", fileInfo.st_atime);
    printf("\t\t\"modificationTime\": %ld,\n", fileInfo.st_mtime);
    printf("\t\t\"statusChangeTime\": %ld,\n", fileInfo.st_ctime);
    printf("\t}\n");
    printf("}\n");
}

int setupLogFile()
{
    
    logFile = fopen(logFileName, "w+");
    logMsg("log initialization start\n");
    if (logFile == NULL)
    {
        printf("Unable to access log file");
        return -1;
    }
    
    logMsg("  log file is ready\n");
    logReady = 1;
    free(logBuffer);

    return 0;
}

int checkForFlags(char * arg)
{
    return  !(  strcmp(arg, "-i")   ||  strcmp(arg, "--inode")  ||
                strcmp(arg, "-?")   ||  strcmp(arg, "--help")   ||
                strcmp(arg, "-a")   ||  strcmp(arg, "--all")    ||
                strcmp(arg, "-h")   ||  strcmp(arg, "--human")  ||
                strcmp(arg, "-f")   ||  strcmp(arg, "--format") ||
                strcmp(arg, "-l")   ||  strcmp(arg, "--log")    
            );
}

void logMsg(char * msg)
{
    if (! logReady)
    {
        strcat(logBuffer, msg);
    }
    else if (logMode)
    {
        fprintf(logFile, "%s", msg);
    }
}

int checkFlag(char * a, char * b)
{
    return strcmp(a, b) == 0;
}