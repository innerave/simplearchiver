#include "archiver.h"

//создаем архив и возвращаем его дескриптор
int create_arch(char *arch_name) {
        int arch_fd;
        if (access(arch_name, F_OK) == 0) {
                int c;
                printf("Файл существует, перезаписать? [y/n]: ");
                c = toupper(getchar());
                int answ = c;
                while (c != '\n' && c != EOF) c = getchar();
                if (answ != 'Y') {
                        errno = EEXIST;
                        perror("Ошибка существования файла");
                        return -1;
                }
        }
        arch_fd = creat(arch_name, DIR_MODE);
        if (arch_fd == -1) perror("Ошибка создания архива");
        return arch_fd;
}

//записываем конечный нулевой хэдэр и закрываем архив
int end_of_arch(int arch_fd) {
        struct meta_data header = {0};
        if (write(arch_fd, &header, sizeof(struct meta_data)) == -1) {
                perror("Ошибка записи");
                return 1;
        }
        close(arch_fd);
        return 0;
}

int write_to_arch(int arch_fd, char *name) {
        struct stat fstat;
        stat(name, &fstat);
        if (access(name, F_OK) == -1) {
                perror("Ошибка существование файла");
                return -1;
        }
        //смотрим если директория
        if (S_ISDIR(fstat.st_mode)) {
                char path[PATH_MAX];
                strcpy(path, name);
                //дописываем путь до нужного формата
                if (path[strlen(name) - 1] != '/') strcat(path, "/");
                //записываем директорию
                write_dir_to_arch(arch_fd, path);
        } else {
                //записываем файл
                write_file_to_arch(arch_fd, name);
        }
        return 0;
}

int write_file_to_arch(int arch_fd, char *file) {
        struct meta_data header;
        struct stat fstat;
        stat(file, &fstat);
        //формируем хэдер
        strcpy(header.name, file);
        header.size = fstat.st_size;
        header.mode = fstat.st_mode;
        //записываем хэдер
        if (write(arch_fd, &header, sizeof(struct meta_data)) == -1) {
                perror("Ошибка записи");
                return 1;
        }
        //открываем файл, который будем архивировать
        int fd = open(file, O_RDONLY);
        if (fd == -1) {
                perror("Ошибка открытия файла");
                return 2;
        }
        //поблочно! (BUF_SIZE) переписываем из файла в архив
        void *buf[BUF_SIZE];
        ssize_t len_r, len_w;
        while ((len_r = read(fd, buf, BUF_SIZE)) > 0) {
                len_w = write(arch_fd, buf, len_r);
                if (len_r != len_w) {
                        perror("Ошибка копирования файла");
                        return 3;
                }
        }

        close(fd);
        return 0;
}

//селектор, фильтруем ссылки на преддериктории
static int selector(const struct dirent *entry) {
        if (strcmp(entry->d_name, ".") == 0) return 0;
        if (strcmp(entry->d_name, "..") == 0) return 0;
        return 1;
}

int write_dir_to_arch(int arch_fd, char *dir) {
        struct meta_data header = {0};
        struct stat dstat;
        //формируем хэдер
        stat(dir, &dstat);
        strcpy(header.name, dir);
        header.mode = dstat.st_mode;
        //записываем хэдер
        if (write(arch_fd, &header, sizeof(struct meta_data)) == -1) {
                perror("Ошибка записи");
                return 1;
        }
        //тут кароч низкоуровнего обходим папку (без потоков)
        struct dirent **eps;
        //получаем список элеметов уровня dir (alphasort - стандартная функция)

        int n = scandir(dir, &eps, selector, alphasort);
        if (n >= 0) {
                int cnt;
                for (cnt = 0; cnt < n; ++cnt) {
                        char path[PATH_MAX];
                        strcpy(path, dir);
                        //eps[cnt]->d_name - текущий элемент
                        strcat(path, eps[cnt]->d_name);
                        //обрабатываем новый элемент
                        // (тк может быть как файлом так и папкой)
                        write_to_arch(arch_fd, path);
                }
        } else
                perror("Ошибка открытия директории");

        return 0;
}

int extract_from_arch(char *arch_name) {
        int arch_fd;
        //проверка на существование архива
        if (access(arch_name, F_OK) == -1) {
                perror("Ошибка существования файла");
                return 1;
        }
        arch_fd = open(arch_name, O_RDONLY);
        if (arch_fd == -1) {
                perror("Ошибка открытия файла");
                return 2;
        }

        struct meta_data header;
        ssize_t len;
        // обрабатываем файлы
        while (len = read(arch_fd, &header, sizeof(struct meta_data)) > 0) {
                if (header.size == 0 && strlen(header.name) == 0) break;
                //проверка на конец архива
                printf("Извлечение: %s\n", header.name);
                if (S_ISDIR(header.mode)) {
                        extract_dir(arch_fd, header.name);
                } else {
                        extract_file(arch_fd, header);
                }
        }

        close(arch_fd);
        return 0;
}

int extract_dir(int arch_fd, char *name) {
        if (access(name, F_OK) == 0) {
                printf("Папка существует\n");
                return 0;
        }
        if (mkdir(name, DIR_MODE) == -1) {
                perror("Ошибка создания папки");
                return 1;
        }
        return 0;
}

int extract_file(int arch_fd, struct meta_data header) {
        char *name = header.name;
        off_t size = header.size;
        int fd;
        //если файл существует
        if (access(name, F_OK) == 0) {
                int c;
                printf("Файл существует, перезаписать? [y/n]: ");
                c = toupper(getchar());
                int answ = c;
                while (c != '\n' && c != EOF) c = getchar();
                if (answ != 'Y') {
                        if (lseek(arch_fd, size, SEEK_CUR) == -1) {
                                perror("Ошибка чтения архива");
                                //аварийное прерывание тк,
                                //последущая разархивация невозможна
                                abort();
                        }
                        return 1;
                }
        }
        //создаем или перезаписываем файл
        fd = creat(name, DIR_MODE);
        //побитово копируем из архива в созданный файл size-байтов
        while (size > 0) {
                ssize_t len_r, len_w;
                const off_t BUF_SIZE_CURR =
                        (size < BUF_SIZE) ? size : BUF_SIZE;
                void *buf[BUF_SIZE_CURR];
                len_r = read(arch_fd, buf, BUF_SIZE_CURR);
                len_w = write(fd, buf, len_r);
                if (len_r != len_w) {
                        perror("Ошибка копирования файла");
                        return 2;
                }
                size -= BUF_SIZE_CURR;
        }

        close(fd);
        return 0;
}
