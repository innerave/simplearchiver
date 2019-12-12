#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
//обработка одного файла 
//(добавляем размер и название в pre_info.meta 
// а данные файла в pre_arch)
int add_file_meta(char *path);
//создание архива(хз поч так назвал, переиминуйте)
int make_meta(char *name,char **argv,int optind,const int argc);
//добавление метаинформации и тела архива в конечный файл
int make_arch(char *name);
//добавляем в info.meta количество файлов (sizeof(int),1)
//добавляем метаинформацию в info.meta
int add_files_count(int files);
//копируем побитово из одного файлового потока в другой
//возвращаем количество скопированных битов
long int copy_open_file(FILE *from,FILE *to);
//то же что и выше, только n битов
long int copy_open_file_by_n(FILE *from,FILE *to,long int n);
/*надо изменить названия
	создаем info.meta с метаинформацией 
	и пофайлово сохраняем из архива*/
int take_meta(char *name);
/*сохранить файл из arch с именем и размером из info
по пути path (пока path приписывает вначале имени файла строку)*/
int save_file(char *path,FILE *arch,FILE *info);