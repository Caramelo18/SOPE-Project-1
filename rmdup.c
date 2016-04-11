#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define FALSE 0
#define TRUE 1
#define BUF_LENGTH 256


struct fileInfo {
        char name[25];
        char path[50];
        unsigned int permissionAccess;
        unsigned long date;
};

void getFileInfo(struct fileInfo *info)
{
        FILE *files = fopen("files.txt", "r");
        if(files == NULL)
        {
           perror("files.txt");
           printf("%d\n", errno);
        }
        char line[115];

        int i = 0;
        while(fgets(line, 115, files)!=NULL)
        {
          char name[25];
          char path[50];
          char permissions[6];
          char date[12];

          memcpy(name, &line[0], 23);
          strcpy(info[i].name, name);

          memcpy(path, &line[28], 47);
          strcpy(info[i].path, path);

          memcpy(permissions, &line[85], 6);
          int perm = strtol(permissions, NULL, 10);
          info[i].permissionAccess = perm;

          memcpy(date, &line[96], 13);
          int dat = strtol(date, NULL, 10);
          info[i].date = dat;

          i++;
        }
        fclose(files);
        return;
}

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
        int temp_std_out = dup(STDOUT_FILENO);
        int temp_std_in = dup(STDIN_FILENO);

        if(argc != 2)
        {
                fprintf(stderr, "Usage: %s <dir_name> \n", argv[0]);
                exit(1);
        }

        FILE *files = fopen("files.txt", "w");
        fclose(files);

        pid_t pid;
        if((pid=fork())==0)
                execlp("./lsdir", "./lsdir", argv[1], NULL);

        int status;
        wait(&status);
        //printf("AKKKKI AO LUAR AO PE DE TI AO PE DO MAR, SO O SONHO FICA SO ELE PODE FICAR\n");

        int file = open("files.txt", O_RDWR);
        dup2(file,STDIN_FILENO);
        dup2(file, STDOUT_FILENO);

        if((pid = fork())==0)
                execlp("sort", "sort", "files.txt", NULL);
        wait(&status);
        close(file);

        struct fileInfo info[256];

        dup2(temp_std_out,STDOUT_FILENO);
        dup2(temp_std_in, STDIN_FILENO);

        getFileInfo(info);

        return 0;
}
