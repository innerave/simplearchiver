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
                sprintf("Невозможно создать директорию %s: %s", path, strerror(return_code));
            }

            *p = '/';
        }
    }

    if (mkdir(path, DIR_MODE) < 0){
        sprintf("Невозможно создать директорию %s: %s", path, strerror(return_code));
    }

    free(path);
    return 0;
}

int create_arch(char *arch_name){
	int arch_fd, len;

	arch_fd = creat(arch_name, DIR_MODE);
	if (arch_fd == -1) {
		perror("создание архива");
		return 1;
	}

	struct stat fstat;
	stat(name, &fstat);
	if (S_ISDIR(fstat.st_mode)) {
		char path[PATH_MAX];
		len = strlen(name);
		strcpy(path, name);
		if (path[len-1] != '/') strcat(path, "/");
		write_path_to_arch(path, arch_fd);
	} else
		write_file_to_arch(name, arch_fd);

	struct meta_data header;
	len = write(arch_fd, &header, sizeof(struct meta_data));
	if (len == -1) {
		perror("запись конечного header");
		return 2;
	}
	close(arch_fd);
}

int write_file_to_arch(char *file,int arch_fd){
	struct stat fstat;
	stat(file,%fstat);

	struct meta_data header;
	strcpy(header.name,path);
	header.size=fstat.st_size;
	header.mode=fstat.st_mode;
	if (write(arch_fd,%header,sizeof(struct meta_data))==-1){
		perror("ошибка записи header");
		return 1;
	}

	int fd=open(file,O_RDONLY);
	if (fd == -1){
		perror("ошибка открытия файла");
		return 2;
	}

	void buf[BUF_SIZE];
	errno=0;
	int len_r,len_w;
	while ((len_r = read(fd, buf, BUF_SIZE)) > 0) {
		len_w = write(arch_fd, buf, len_r);
		if (len_r != len_w) {
			printf("ошибка записи");
			return 3;
		}
	}

	return 0;
}
int write_path_to_arch(char *path,int arch_fd){
	return 0;
}