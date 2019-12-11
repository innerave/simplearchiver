#include "archiver.h"

int make_meta(char *name,char **argv,int optind,int argc)
{
	int files=argc-optind;
    for(; optind < argc; optind++){
        char *arg=argv[optind];
        printf("Выбран файл: %s\n", arg);
        if(add_file_meta(arg)==-1) {		//обрабатываем очередной файл
        	printf("Невозможно открыть %s\n",arg);
        	files--;
        }
    }
    add_files_count(files);
    make_arch(name,files);
    return 1;
}
int make_arch(char *name,int file)
{
	FILE *info=fopen("info.meta","rb");
	if(info==NULL) return -1;	
	FILE *pre_arch=fopen("pre_arch.meta","rb");
	if(pre_arch==NULL) {
		fclose(info);
		return -1;
	}	
	FILE *arch=fopen(name,"wb");
	if(pre_arch==NULL) {
		fclose(info);
		fclose(pre_arch);
		return -1;
	}
	copy_open_file(info,arch); 			//копируем метаинформацию
	copy_open_file(pre_arch,arch);		//копируем тело архива
	fclose(info);fclose(pre_arch);fclose(arch);
	remove("pre_arch.meta");remove("info.meta");		//подчищаем все говно
	return 1;
}
int add_file_meta(char *path) 
{	
	//ХЗ КАК ТУТ (относится ко всем таким блокам)
	FILE *to_read=fopen(path,"rb");
	if (to_read==NULL) return -1;
	FILE *pre_info=fopen("pre_info.meta","ab");
	if (pre_info==NULL) {
		fclose(to_read);
		return -1;
	}
	FILE *pre_arch=fopen("pre_arch.meta","ab");
	if (pre_arch==NULL) {
		fclose(to_read);
		fclose(pre_info);
		return -1;
	}
	//------------------------------------------
	int size = copy_open_file(to_read,pre_arch);	//записываем весь файл в конец pre_arch
	printf("размер %d\n", size);
	fwrite(path,sizeof(char),80,pre_info);			//сохраняем метаинформацию ( имя файла , размер файла)
	fwrite(&size,sizeof(int),1,pre_info);			//

	fclose(to_read);fclose(pre_info);fclose(pre_arch);
	return 1;
}
int add_files_count(int files)
{
	FILE *pre_info=fopen("pre_info.meta","rb");
	if (pre_info==NULL) return -1;
	FILE *info=fopen("info.meta","ab");
	if (info==NULL) {
		fclose(pre_info);
		return -1;
	}	

	fwrite(&files,sizeof(int),1,info);		//добавляем в info.meta сначала количество файлов
	copy_open_file(pre_info,info);			//потом метаинформацию ( имя файла , размер файла)

	fclose(pre_info);fclose(info);
	remove("pre_info.meta");
	return 1;
}

int copy_open_file(FILE *from,FILE *to)
{
	int size=0;
	char buf[1];
	while(1) {
		// поБИТово копируем сначала ноль, потом целковый, полушка и т.д.
    	fread(buf,sizeof(char),1,from);
    	size++;
    	if (feof(from)) break;
    	fwrite(buf,sizeof(char),1,to);
  	}
  	return size;
}