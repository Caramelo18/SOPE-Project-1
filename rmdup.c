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

void writeLinks(struct fileInfo info){
        FILE *files = fopen("hlinks.txt", "a");
        char name[200];
        sprintf(name, "%-20s %-30s\n", info.name, info.path);
        fprintf(files, "%s", name);
        fclose(files);
}

int getFileInfo(struct fileInfo *info)
{
        FILE *files = fopen("files.txt", "r");
        if(files == NULL)
        {
                perror("files.txt");
                printf("%d\n", errno);
        }
        char line[150];

        int i = 0;
        while(fgets(line, 150, files)!=NULL)
        {
                char *name;
                char *path;
                char *permissions;
                char *date;
                const char sep[3] = "**";

                name = strtok(line, sep);
                date = strtok(NULL, sep);
                path = strtok(NULL, sep);
                permissions = strtok(NULL, sep);
                permissions = strtok(NULL, sep);

                strcpy(info[i].name, name);

                int dat = strtol(date, NULL, 10);
                info[i].date = dat;

                strcpy(info[i].path, path);

                int perm = strtol(permissions, NULL, 10);
                info[i].permissionAccess = perm;

                i++;
        }
        fclose(files);
        return i;
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


int isDup(struct fileInfo f1, struct fileInfo f2)
{
        char name1[80];
        char name2[80];
        sprintf(name1, "%s/%s", f1.path, f1.name);
        sprintf(name2, "%s/%s", f2.path, f2.name);

        if(strcmp(f1.name, f2.name) != 0)
                return FALSE;

        if(f1.permissionAccess != f2.permissionAccess)
                return FALSE;
        return hasSameContent(name1, name2);
}

void checkDupFiles(struct fileInfo *info, int size)
{
        int i, j;
        for(i = 0; i < size; i++)
        {
                for(j = i + 1; j < size; j++)
                {
                        int result;
                        if((result = isDup(info[i], info[j])) == TRUE) {

                                pid_t pid;
                                if((pid=fork())==0){
                                  char file1[200];
                                  char file2[200];
                                  sprintf(file1, "%s/%s", info[i].path, info[i].name);
                                  sprintf(file2, "%s/%s", info[j].path, info[j].name);
                                        if(info[i].date < info[j].date) {
                                              writeLinks(info[j]);

                                            //  if((pid=fork()==0))
                                            //    execlp("rm", "rm", "%s", file2);
                                            //  wait(&status);
                                              execlp("ln", "ln", "-f", file1, file2, NULL);

                                        }else if(info[i].date > info[j].date){
                                              writeLinks(info[i]);
                                              execlp("ln", "ln", "-f", file2, file1, NULL);
                                        }
                                }
                        }
                        else if (result == FALSE);
                }
        }
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
        files = fopen("hlinks.txt", "w");
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

        int number = getFileInfo(info);

        checkDupFiles(info, number);

        return 0;
}
