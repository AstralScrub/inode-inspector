#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define PLAIN_TEXT  (0)
#define JSON        (1)

struct stat fileInfo;
char * fileName;
int format;


void argsHandler(int argc, char * argv[]);
void help(int argc, char * argv[]);

int main(int argc, char *argv[]) {
    
    
    argsHandler(argc, argv);
    
    printf("Information for %s:\n", fileName);
    printf("File Inode: %lu\n", fileInfo.st_ino);
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
        format = PLAIN_TEXT;
        fileName = argv[1];
    }
    else
    {
        fileName = argv[0];
    }
    if (stat(fileName, &fileInfo) != 0) {
        fprintf(stderr, "Error getting file info for %s: %s\n", argv[1], strerror(errno));
        exit(1);
    }
}