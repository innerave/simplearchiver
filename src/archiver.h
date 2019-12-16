#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#define DIR_MODE S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
/*	файловые биты используются для определения типа файла.
	S_IRWXU чтение, запись, выполнение / поиск владельцем
	S_IRGRP разрешение на чтение
	S_IXGRP разрешение на выполнение / поиск 
*/
// рекурсивное создание директорий
int r_mkdir(const char * dir);