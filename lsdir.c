#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>


int main(int argc, char* argv[]){

        DIR *dir;
        struct dirent *direntp;
        struct stat statbuf;
        pid_t pid;
        int status;

        if((dir = opendir(argv[1])) == NULL)
        {
                perror(argv[1]);
                exit(2);
        }

        while((direntp = readdir(dir)) != NULL)
        {
                char name[200];
                if(strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0 || strcmp(direntp->d_name, ".git") == 0) {}
                else{
                        sprintf(name, "%s/%s", argv[1], direntp->d_name);
                        if(lstat(name, &statbuf) == -1)
                        {
                                perror("lstat error");
                                exit(3);
                        }
                        if(S_ISREG(statbuf.st_mode))
                        {
                                FILE *files = fopen("files.txt", "a");
                                sprintf(name, "%s**%d**%s**%d**%d\n",
                                      direntp->d_name, (int)statbuf.st_mtime, argv[1], (int)statbuf.st_size, statbuf.st_mode);
                                fprintf(files,"%s", name);
                                fclose(files);
                        }
                        else if(S_ISDIR(statbuf.st_mode))
                        {
                                pid = fork();
                                if(pid == 0) {
                                        execlp("./lsdir", "./lsdir", name, NULL);
                                }
                                else   wait(&status);

                        }
                }
        }


        closedir(dir);
        return 0;
}
