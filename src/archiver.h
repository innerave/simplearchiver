#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <utime.h>
#include <time.h>

#define BUF_SIZE 2048

enum Mode {
        HELP = 1, CREATE, EXTRACT
};

struct meta_data {
        char   name[PATH_MAX];
        off_t  size;
        mode_t mode;
};


#define DIR_MODE S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

int create_arch(char *arch_name);

int write_to_arch(int arch_fd, char *name);

int write_file_to_arch(int arch_fd, char *file);

int write_dir_to_arch(int arch_fd, char *dir);

int end_of_arch(int arch_fd);

int extract_from_arch(char *arch_name);

int extract_dir(int arch_fd, char *name);

int extract_file(int arch_fd, struct meta_data header);