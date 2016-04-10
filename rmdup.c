#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define FALSE 0
#define TRUE 1
#define BUF_LENGTH 256

int hasSameContent(char name1[], char name2[])
{
        FILE *f1, *f2;
        char buf[BUF_LENGTH], buf1[BUF_LENGTH];
        if((f1 = fopen(name1, "r")) == NULL)
        {
                perror(name1);
                printf("%d\n", errno);
                exit(2);
        }
        if((f2 = fopen(name2, "r")) == NULL)
        {
                perror(name2);
                printf("%d\n", errno);
                exit(2);
        }

        while((fgets(buf, BUF_LENGTH, f1) != NULL) && fgets(buf1, BUF_LENGTH, f2) != NULL)
        {
                if(strcmp(buf, buf1) != 0)
                        return FALSE;
        }

        return TRUE;
}

int isDup(char path1[], char file1[], char path2[], char file2[])
{
        char name1[30];
        char name2[30];
        struct stat stat1, stat2;

      //  if(file1 != file2)
     //     return FALSE;
        sprintf(name1, "%s/%s", path1, file1);
        sprintf(name2, "%s/%s", path2, file2);

        printf("%s\n", name1);
        printf("%s\n", name2);

        stat(name1, &stat1);
        stat(name2, &stat2);

        if(stat1.st_mode != stat2.st_mode)
                return FALSE;

        return hasSameContent(name1, name2);
}


int main(int argc, char* argv[])
{
        if(argc != 2)
        {
                fprintf(stderr, "Usage: %s <dir_name> \n", argv[0]);
                exit(1);
        }

      FILE *files = fopen("files.txt", "w");
      fclose(files);

      execlp("./lsdir", "./lsdir", argv[1], NULL);

      /*int file = open("files.txt", O_RDWR);
      dup2(file,STDIN_FILENO);//, file);
      close(file);

      file = open("files.txt", O_RDWR);
      dup2(file, STDOUT_FILENO);
      execlp("sort", "sort", "files.txt", NULL);
      close(file);
        closedir(dir*/
        return 0;
}
