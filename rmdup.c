#include "utilities.h"

struct fileInfo {
        char name[NAME_SIZE];
        char path[BUF_LENGTH];
        unsigned int permissionAccess;
        unsigned long date;
        int duplicated;
};

//Writes information from hard links to hlinks.txt
void writeLinks(struct fileInfo target, struct fileInfo linkName){
        FILE *files = fopen("hlinks.txt", "a");
        char name1[BUF_LENGTH];
        char name2[BUF_LENGTH];
        sprintf(name1, "%s/%s", target.path, target.name);
        sprintf(name2, "%s/%s", linkName.path, linkName.name);
        fprintf(files, "Linked from source %s to target %s\n", name2, name1);
        fclose(files);
}

//Creates a hard link
void createHardLink(struct fileInfo target, struct fileInfo linkName)
{
        char trgt[BUF_LENGTH];
        char lknm[BUF_LENGTH];

        sprintf(trgt, "%s/%s", target.path, target.name);
        sprintf(lknm, "%s/%s", linkName.path, linkName.name);

        writeLinks(target, linkName);
        //The way files.txt is ordered the first file is always older so there is no need to compare
        execlp("ln", "ln", "-f", trgt, lknm, NULL);
}

//Returns number of files' information in files.txt
int getFileInfo(struct fileInfo *info)
{
        FILE *files = fopen("files.txt", "r");
        if(files == NULL)
        {
                perror("files.txt");
                printf("%d\n", errno);
        }
        char line[BUF_LENGTH];

        int i = 0;
        while(fgets(line, BUF_LENGTH, files)!=NULL)
        {
                char *name;
                char *path;
                char *permissions;
                char *date;
                const char sep[3] = "**";

                name = strtok(line, sep);
                date = strtok(NULL, sep);
                path = strtok(NULL, sep);
                permissions = strtok(NULL, sep);   //Gets size but it is not important for comparisons
                permissions = strtok(NULL, sep);

                strcpy(info[i].name, name);

                int dat = strtol(date, NULL, BASE_10);
                info[i].date = dat;

                strcpy(info[i].path, path);

                int perm = strtol(permissions, NULL, BASE_10);
                info[i].permissionAccess = perm;

                i++;
                info[i].duplicated = FALSE;
        }
        fclose(files);
        return i;
}

//Checks the content of file name1 and file2 and returns TRUE if the contents are equal or FALSE if not
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
        int result = -1;
        char *temp1;
        char *temp2;
        while(result == -1)
        {
            temp1 = fgets(buf, BUF_LENGTH, f1);
            temp2 = fgets(buf1, BUF_LENGTH, f2);
            if((temp1 == NULL && temp2 != NULL) || (temp1 != NULL && temp2 == NULL))
                    result = FALSE;
            else if(temp1==NULL && temp2 == NULL)
                result =TRUE;
            else if(strcmp(buf, buf1) != EQUAL)
                result = FALSE;

        }
        fclose(f1);
        fclose(f2);
        return result;
}

//Returns TRUE if the contents between f1 and f2 are the same and FALSE if not
int isDup(struct fileInfo f1, struct fileInfo f2)
{
        if(strcmp(f1.name, f2.name) != EQUAL)
              return FALSE;

        if(f1.permissionAccess != f2.permissionAccess)
              return FALSE;

        char name1[BUF_LENGTH];
        char name2[BUF_LENGTH];
        sprintf(name1, "%s/%s", f1.path, f1.name);
        sprintf(name2, "%s/%s", f2.path, f2.name);

        return hasSameContent(name1, name2);
}


//Creates hard links for files that are equal in the estabilished parameters
void checkDupFiles(struct fileInfo *info, int size)
{
        int i, j;
        for(i = 0; i < size; i++)
        {
                for(j = i + 1; j < size; j++)
                {
                        if(isDup(info[i], info[j]) == TRUE)
                        {
                                if(info[j].duplicated == FALSE) {
                                        info[j].duplicated = TRUE;
                                        info[i].duplicated = TRUE;
                                        pid_t pid;
                                        if((pid=fork()) == 0) { //If child process
                                                createHardLink(info[i], info[j]);
                                        }
                                }
                        }
                }
        }
}

int main(int argc, char* argv[])
{
        //Creates backup of important descriptors
        int temp_std_out = dup(STDOUT_FILENO);
        int temp_std_in = dup(STDIN_FILENO);

        if(argc != 2)
        {
                fprintf(stderr, "Usage: %s <dir_name> \n", argv[0]);
                exit(1);
        }

        //Ensures that the files are empty
        FILE *files = fopen("files.txt", "w");
        fclose(files);
        files = fopen("hlinks.txt", "w");
        fclose(files);
        files = fopen("log.txt", "w");
        fclose(files);

        pid_t pid;
        if((pid=fork())==0) //If child process
                execlp("./lsdir", "./lsdir", argv[1], NULL);

        int status;
        wait(&status);

        int file = open("files.txt", O_RDWR);
        dup2(file,STDIN_FILENO);
        dup2(file, STDOUT_FILENO);

        if((pid = fork())==0)
                execlp("sort", "sort", "files.txt", NULL);  //Sort is done by the following order
                                                            //    1-filename
                                                            //    2-date of last modification
                                                            //    3-path of file
                                                            //    4-size of file
                                                            //    5-permission access
        wait(&status);
        close(file);

        struct fileInfo info[BUF_LENGTH];

        //Restores the standart STD output and input
        dup2(temp_std_out,STDOUT_FILENO);
        dup2(temp_std_in, STDIN_FILENO);


        int number = getFileInfo(info);

        checkDupFiles(info, number);

        return 0;
}
