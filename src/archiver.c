#include "archiver.h"


// рекурсивное создание директорий
int r_mkdir(const char * dir){
    int return_code = 0;
    const size_t len = strlen(dir);

    if (!len){
        return 0;
    }

    char * path = calloc(len + 1, sizeof(char));
    strncpy(path, dir, len);

    // удаление '/'
    if (path[len - 1] ==  '/'){
        path[len - 1] = 0;
    }

    // все последующие каталоги не существуют
    for(char * p = path + 1; *p; p++){
        if (*p == '/'){
            *p = '\0';

            if ((return_code = mkdir(path, DIR_MODE))){
                printf("Невозможно создать директорию %s: %s", path, strerror(return_code));
            }

            *p = '/';
        }
    }

    if (mkdir(path, DIR_MODE) < 0){
        printf("Невозможно создать директорию %s: %s", path, strerror(return_code));
    }

    free(path);
    return 0;
}
//создаем архив и ворзвращаем его дескриптор
int create_arch(char *arch_name){
	int arch_fd = creat(arch_name, DIR_MODE);
	if (arch_fd == -1) {
		perror("создание архива");
		return 1;
	}
	return arch_fd;
}
//записываем конечный нулевой хэдэр и закрываем архив
int end_of_arch(int arch_fd){
	struct meta_data header={0};
	if ( write(arch_fd, &header, sizeof(struct meta_data)) == -1) {
		perror("запись конечного header");
		return 2;
	}
	close(arch_fd);
	return 0;
}

int write_to_arch(int arch_fd, char *name){
	struct stat fstat;
	stat(name, &fstat);

	if (S_ISDIR(fstat.st_mode)) {				//смотрим если директория
		char path[PATH_MAX];
		strcpy(path, name);
		if (path[strlen(name)-1] != '/') strcat(path, "/");		//дописываем путь до нужного формата
		write_dir_to_arch(arch_fd, path);				//записываем директорию
	} else {
		write_file_to_arch(arch_fd, name);				//записываем файл
	}
	return 0;
}

int write_file_to_arch(int arch_fd,char *file){
	struct meta_data header;
	struct stat fstat;
	stat(file, &fstat);
	//формируем хэдер
	strcpy(header.name,file);	
	header.size=fstat.st_size;
	header.mode=fstat.st_mode;
	//записываем хэдер
	if (write(arch_fd, &header,sizeof(struct meta_data))==-1){
		perror("ошибка записи header");
		return 1;
	}
	//открываем файл, который будем архивировать
	int fd=open(file,O_RDONLY);
	if (fd == -1){
		perror("ошибка открытия файла");
		return 2;
	}
	//поблочно (BUF_SIZE) переписываем из файла в архив
	void *buf[BUF_SIZE];
	int len_r,len_w;
	while ((len_r = read(fd, buf, BUF_SIZE)) > 0) {
		len_w = write(arch_fd, buf, len_r);
		if (len_r != len_w) {
			printf("ошибка записи");
			return 3;
		}
	}

	close(fd);
	return 0;
}
//селектор, фильтруем ссылки на преддериктории
static int selector(const struct dirent *entry){
	if (strcmp(entry->d_name, ".") == 0) return 0;
	if (strcmp(entry->d_name, "..") == 0) return 0;
	return 1;
}

int write_dir_to_arch(int arch_fd,char *dir){
	struct meta_data header;
	struct stat dstat;
	//формируем хэдер
	stat(dir, &dstat);
	strcpy(header.name, dir);
	header.size=0;
	header.mode=dstat.st_mode;
	//записываем хэдер
	if (write(arch_fd, &header, sizeof(struct meta_data))==-1){
		perror("ошибка записи header");
		return 1;
	}
	//тут кароч низкоуровнего обходим папку (без потоков)
 	struct dirent **eps;
 	//смысл шо получаем список элеметов уровня dir (alphasort - стандартная функция , есть еще timesort)
	int n = scandir (dir, &eps, selector, alphasort);
 	if (n >= 0)
 	  {
 	    int cnt;			
 	    for (cnt = 0; cnt < n; ++cnt){
			char path[PATH_MAX];
			strcpy(path, dir);
			strcat(path, eps[cnt]->d_name);				//eps[cnt]->d_name - текущий элемент
 	    	write_to_arch(arch_fd, path);			//обрабатываем новый элемент (тк может быть как файлом так и папкой)
 	    }
 	  }
 	else
 	  perror ("Couldn't open the directory");

	return 0;
}