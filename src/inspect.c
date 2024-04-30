#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

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

void argsHandler(int argc, char * argv[]);
void help(int argc, char * argv[]);

int main(int argc, char *argv[]) {
    
    
    argsHandler(argc, argv);
    
    if (stat(fileName, &fileInfo) != 0) 
    {
        fprintf(stderr, "Error getting file info for %s: %s\n", argv[1], strerror(errno));
        exit(1);
    }
    
    printf("Information for %s:\n", fileName);
    printf("File Inode: %lu\n", fileInfo.st_ino);
    printf("Premission: ");
    if(S_IRUSR&fileInfo.st_mode)
        printf("r-");
    if(S_IWUSR&fileInfo.st_mode)
        printf("w-");
    if(S_IXUSR&fileInfo.st_mode)
        printf("x-");
    if(S_IRGRP&fileInfo.st_mode)
        printf("r--");
    if(S_IRGRP&fileInfo.st_mode)
        printf("w--");
    if(S_IRGRP&fileInfo.st_mode)
        printf("x--");
    if(S_IROTH&fileInfo.st_mode)
        printf("r---");
    if(S_IROTH&fileInfo.st_mode)
        printf("w---");
    if(S_IROTH&fileInfo.st_mode)
        printf("x---");
    printf("\n");
    printf("File Type: ");
    if (S_ISREG(fileInfo.st_mode))
        printf("regular file\n");
    else if (S_ISDIR(fileInfo.st_mode))
        printf("directory\n");
    else if (S_ISCHR(fileInfo.st_mode))
        printf("character device\n");
    else if (S_ISBLK(fileInfo.st_mode))
        printf("block device\n");
    else if (S_ISFIFO(fileInfo.st_mode))
        printf("FIFO (named pipe)\n");
    else if (S_ISLNK(fileInfo.st_mode))
        printf("symbolic link\n");
    else if (S_ISSOCK(fileInfo.st_mode))
        printf("socket\n");
    else
        printf("unknown?\n");

    
    printf("Number of Hard Links: %lu\n", fileInfo.st_nlink);
    printf("File Size: %lu bytes\n", fileInfo.st_size);
    printf("Last Access Time: %ld\n", fileInfo.st_atime);
    printf("Last Modification Time: %ld\n", fileInfo.st_mtime);
    printf("Last Status Change Time: %ld\n", fileInfo.st_ctime);

    return 0;
}

void help(int argc, char * argv[])
{
    printf("%s: %s [-? | -i <file_path> | -a [directory_path]] [-a | -f [text|json]] [-l]\n", argv[0], argv[0]);
}

void argsHandler(int argc, char * argv[])
{
    if (argc == 1)
    {
        help(argc, argv);
        exit(1);
    }
    else if (argc == 2)
    {
        format = FORMAT_TEXT;
        selectionMode = SELECTION_SINGLE;
        fileName = argv[1];
    }
    else
    {
        format = -1;
        selectionMode = -1;
        fileName = argv[0];
        
        for (int i = 1; i < argc; i++)
        {
            char * arg = argv[i];
            if (strcmp(arg, "-i") == 0)
            {
                printf("i\n");
                selectionMode = SELECTION_SINGLE;
                if (i + 1 < argc)
                    fileName = argv[i+1];
            } 
            else if (strcmp(arg, "-a") == 0)
            {
                printf("a\n");
                selectionMode = SELECTION_SINGLE;
                fileName = argv[i+1];

                if (i + 2 < argc)
                {
                    if (strcmp(argv[i+2], "-r"))
                    {
                        selectionMode = SELECTION_RECURSIVE;
                    }
                }
            }
            
        }
    }
}