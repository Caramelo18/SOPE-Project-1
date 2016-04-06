#include <time.h>
#include <unistd.h>

void lsdir(char path[])
{
  FILE *files = fopen("files.txt", "w");
  DIR *dir;
  struct dirent *direntp;
  struct stat statbuf;

  if((dir = opendir(path)) == NULL)
  {
    perror(path);
    exit(2);
  }

  char name[200];
  while((direntp = readdir(dir)) != NULL)
  {
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
  /*  else if(S_ISDIR(statbuf.st_mode))
    {
      pid_t pid = fork();
      if(pid == 0)
        lsdir(name);
    }*/
  }
  fclose(files);
}
