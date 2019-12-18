#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>

#define BUF_SIZE 1024
enum Mode{CREATE=1,EXTRACT};
struct meta_data
{
	char name[PATH_MAX];
	off_t size;
	mode_t mode;
};
#define DIR_MODE S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
/*	файловые биты используются для определения типа файла.
	S_IRWXU чтение, запись, выполнение / поиск владельцем
	S_IRGRP разрешение на чтение
	S_IXGRP разрешение на выполнение / поиск 
*/
// рекурсивное создание директорий
int r_mkdir(const char * dir);
int create_arch(char *arch_name);
int write_to_arch(int arch_fd,char *name);
int write_file_to_arch(int arch_fd,char *file);
int write_dir_to_arch(int arch_fd,char *dir);
int end_of_arch(int arch_fd);