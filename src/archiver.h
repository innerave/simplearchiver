#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#define DIR_MODE S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

// рекурсивное создание директорий
int r_mkdir(const char * dir);