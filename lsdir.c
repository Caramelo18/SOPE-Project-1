#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

void lsdir(char path[], FILE *files)
{

        DIR *dir;
        struct dirent *direntp;
        struct stat statbuf;

        if((dir = opendir(path)) == NULL)
        {
                perror(path);
                exit(2);
        }


        while((direntp = readdir(dir)) != NULL)
        {
                char name[200];
                if(strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0 || strcmp(direntp->d_name, ".git") == 0) {}
                else{
                        sprintf(name, "%s/%s", path, direntp->d_name);
                        if(lstat(name, &statbuf) == -1)
                        {
                                perror("lstat error");
                                exit(3);
                        }
                        if(S_ISREG(statbuf.st_mode))
                        {
                                sprintf(name, "%-15s %-20s %-5d %-10d %-10d\n", direntp->d_name, path, (int)statbuf.st_size, statbuf.st_mode, (int)statbuf.st_mtime);
                                fputs(name, files);
                        }
                        else if(S_ISDIR(statbuf.st_mode))
                        {
                              //  sprintf(name, "%-15s %-20s %-5d %-10d %-10d\n", direntp->d_name, path, (int)statbuf.st_size, statbuf.st_mode, (int)statbuf.st_mtime);
                              //  fputs(name, files);
                                pid_t pid = fork();
                                if(pid == 0){
                                        lsdir(name, files);
                                }

                        }
                }
        }
        closedir(dir);
}
