#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

/*void lsdir(char path[], int files)
  {

        DIR *dir;
        struct dirent *direntp;
        struct stat statbuf;
        pid_t pid;

        if((dir = opendir(path)) == NULL)
        {
                perror(path);
                exit(2);
        }

        printf("%-20s  \n", path);
        while((direntp = readdir(dir)) != NULL)
        {
                char name[200];
                //printf("%s/%s\n",path, direntp->d_name );
                if(strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0 || strcmp(direntp->d_name, ".git") == 0) {}
                else{
                        sprintf(name, "%s/%s", path, direntp->d_name);
                        //printf("%s\n ", name);
                        if(lstat(name, &statbuf) == -1)
                        {
                                perror("lstat error");
                                exit(3);
                        }
                        if(S_ISREG(statbuf.st_mode))
                        {
                                dup2(files, STDOUT_FILENO);
                                sprintf(name, "%-15s %-20s %-5d %-10d %-10d\n", direntp->d_name, path, (int)statbuf.st_size, statbuf.st_mode, (int)statbuf.st_mtime);
                                printf("%s", name);
                              //  fprintf(files,"%s", name);
                              //  fputs(name, files);
                        }
                        else if(S_ISDIR(statbuf.st_mode))
                        {
                              //  sprintf(name, "%-15s %-20s %-5d %-10d %-10d\n", direntp->d_name, path, (int)statbuf.st_size, statbuf.st_mode, (int)statbuf.st_mtime);
                              //  fputs(name, files);
                            //  printf("%s \n", name);
                                pid = fork();
                                if(pid == 0){
                                        lsdir(name, files);
                                        return;
                                        //exit(0);
                                        //printf("Forked %d\n", pid);
                                }
                                //printf("Here %d\n", pid);

                        }
                }
        }
        //printf("Exited %d\n", pid);
        closedir(dir);

        return;
}*/

int main(int argc, char* argv[]){

          DIR *dir;
          struct dirent *direntp;
          struct stat statbuf;
          pid_t pid;

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
                                  sprintf(name, "%-15s %-20s %-5d %-10d %-10d\n", direntp->d_name, argv[1], (int)statbuf.st_size, statbuf.st_mode, (int)statbuf.st_mtime);
                                  fprintf(files,"%s", name);
                                  fclose(files);
                          }
                          else if(S_ISDIR(statbuf.st_mode))
                          {
                                  pid = fork();
                                  if(pid == 0){
                                          execlp("./lsdir", "./lsdir", name,NULL);
                                          return 0;
                                  }

                          }
                  }
          }

          int file = open("files.txt", O_RDWR);
          dup2(file,STDIN_FILENO);
          dup2(file, STDOUT_FILENO);
          execlp("sort", "sort", "files.txt", NULL);
          close(file);
          closedir(dir);
          return 0;
}
