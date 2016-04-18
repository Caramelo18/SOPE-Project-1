#include "utilities.h"

int main(int argc, char* argv[]){

        DIR *dir;
        struct dirent *direntp;
        struct stat statbuf;
        pid_t pid;
        pid_t pidList[PID_LIST_SIZE];
        int pidListSize = 0;

        if((dir = opendir(argv[1])) == NULL)
        {
                perror(argv[1]);
                exit(2);
        }

        int endOfDir = FALSE;
        while(endOfDir == FALSE)
        {
                if((direntp = readdir(dir)) != NULL){
                                char name[BUF_LENGTH];
                                //.git folder was ignored so as to not handle sensitive files during creation of project
                                //ignore . and .. folders to prevent endless loops
                                if(strcmp(direntp->d_name, ".") == EQUAL || strcmp(direntp->d_name, "..") == EQUAL || strcmp(direntp->d_name, ".git") == EQUAL) {}
                                else{
                                        sprintf(name, "%s/%s", argv[1], direntp->d_name);
                                        if(lstat(name, &statbuf) == ERROR)
                                        {
                                          if(errno == EACCES){
                                            FILE *files = fopen("log.txt", "a");
                                            fprintf(files,"No permission access to %s\n",name);
                                            fclose(files);

                                          } else{
                                                perror("lstat error");
                                                exit(3);
                                          }
                                        }
                                        // if it is a regular file, stores its information on files.txt
                                        else if(S_ISREG(statbuf.st_mode))
                                        {
                                                FILE *files = fopen("files.txt", "a");
                                                sprintf(name, "%s**%d**%s**%d**%d\n",
                                                        direntp->d_name, (int)statbuf.st_mtime, argv[1], (int)statbuf.st_size, statbuf.st_mode);
                                                fprintf(files,"%s", name);
                                                fclose(files);
                                        }
                                        // if it is a directory launches a new proccess that will search the directory
                                        else if(S_ISDIR(statbuf.st_mode))
                                        {
                                                pid = fork();
                                                if(pid == 0) {
                                                        execlp("./lsdir", "./lsdir", name, NULL);
                                                }
                                                pidList[pidListSize++] = pid;

                                        }
                                }
              } else if(errno ==ENOENT)
                  continue;
                else endOfDir = TRUE;
        }


        closedir(dir);
        int i=0;
        for(;i<pidListSize; i++){
          waitpid(pidList[i], NULL, 0);
        }


        return 0;
}
