#include "archiver.h"

int make_meta(char *name,char **argv,int optind,int argc)
{
	int files=argc-optind; 			//количество фалйов, для архивации
    for(; optind < argc; optind++){
        char *arg=argv[optind];
        printf("Выбран файл: %s\n", arg);
        if(add_file_meta(arg)==-1) {		//обрабатываем очередной файл
        	printf("Невозможно открыть %s\n",arg);
        	files--;					
        }
    }
    add_files_count(files);
    make_arch(name);
    return 1;
}
int make_arch(char *name)
{
	FILE *info=fopen("info.meta","rb");
	if(info==NULL) return -1;	
	FILE *arch_body=fopen("arch_body.meta","rb");
	if(arch_body==NULL) {
		fclose(info);
		return -1;
	}	
	FILE *arch=fopen(name,"wb");
	if(arch_body==NULL) {
		fclose(info);
		fclose(arch_body);
		return -1;
	}
	copy_open_file(info,arch); 			//копируем метаинформацию
	copy_open_file(arch_body,arch);		//копируем тело архива
	fclose(info);fclose(arch_body);fclose(arch);
	remove("arch_body.meta");remove("info.meta");		//подчищаем все говно
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
	FILE *arch_body=fopen("arch_body.meta","ab");
	if (arch_body==NULL) {
		fclose(to_read);
		fclose(pre_info);
		return -1;
	}
	//------------------------------------------
	long int size = copy_open_file(to_read,arch_body);	//записываем весь файл в конец arch_body
	printf("размер %ld\n", size);
	fwrite(path,sizeof(char),80,pre_info);			//сохраняем метаинформацию ( имя файла , размер файла)
	fwrite(&size,sizeof(long int),1,pre_info);			//

	fclose(to_read);fclose(pre_info);fclose(arch_body);
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

long int copy_open_file(FILE *from,FILE *to)
{
	long int size=0;
	int c;
	while(1) {
		// поБИТово копируем сначала ноль, потом целковый, полушка и т.д.
    	c=fgetc(from);
    	if (c==EOF) break;
    	size++;
    	fputc(c,to);
  	}
  	return size; //успешно скопировано битов
}
long int copy_open_file_by_n(FILE *from,FILE *to,long int n)
{
	long int size=0;
	int c;
	for (int i = 0; i < n; ++i)
	{// поБИТово копируем сначала ноль, потом целковый, полушка и т.д.
    	c=fgetc(from);
    	if (c==EOF) break;
    	size++;
    	fputc(c,to);
	}
  	return size; //успешно скопировано битов
}
int take_meta(char *name)
{
	FILE *arch=fopen(name,"rb");
	if (arch==NULL) return -1;
	FILE *info=fopen("info.meta","w+b");
	if (info==NULL){
		fclose(arch);
		return -1;
	}

	int files;
	fread(&files,sizeof(int),1,arch);
	//копируем метоинформацию ( имя файла , размер файла)
	copy_open_file_by_n(arch,info,(80*sizeof(char)+sizeof(long int))*files);
	rewind(info);
	//вытаскиваем пофайлово из архива
	for (int i = 0; i < files; ++i)
		{
			//если вытащить неудалось уменьшаем files
			if (save_file("lol_",arch,info)==-1) files--;
		}	
	fclose(arch);fclose(info);
	remove("info.meta"); //подчищаем
	return files;
}
int save_file(char *path,FILE *arch,FILE *info)
{
	char buf[80];
	char name[160];
	long int size;
	//читаем метаинформацию
	fread(buf,sizeof(char),80,info);
	fread(&size,sizeof(long int),1,info);
	//создаем имя файла (подумать как пути/папки обрабатывать)
	strcpy(name,path);
	strcat(name,buf);
	printf("%s размер %ld\n", name,size);
	//создаем файл
	FILE *file=fopen(name,"wb");
	if (file==NULL) {
		fseek(arch,size,SEEK_CUR);
		return-1;
	}
	//копируем данные
	size=copy_open_file_by_n(arch,file,size);
	printf("copied %ld\n",size);
	fclose(file);
	return 1;
}