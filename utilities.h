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

#define FALSE       0
#define TRUE        1
#define BUF_LENGTH  256
#define NAME_SIZE   128
#define BASE_10     10
#define EQUAL       0

#define PID_LIST_SIZE   128
#define ERROR       -1
