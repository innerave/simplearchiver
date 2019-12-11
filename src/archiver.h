#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
//обработка одного файла 
//(добавляем размер и название в pre_info.meta 
// а данные файла в pre_arch)
int add_file_meta(char *str);
//создание архива(хз поч так назвал, переиминуйте)
int make_meta(char *name,char **argv,int optind,const int argc);
//добавление метаинформации и тела архива в конечный файл
int make_arch(char *name,int file);
//добавляем в info.meta количество файлов (sizeof(int),1)
//добавляем метаинформацию в info.meta
int add_files_count(int files);
//копируем побитово из одного файлового потока в другой
//возвращаем количество скопированных битов
int copy_open_file(FILE *from,FILE *to);