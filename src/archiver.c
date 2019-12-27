/*
 * Простейший архиватор
 */

#include "archiver.h"

/*
 * create_arch() - создание архива
 * @arch_name:  Имя архива
 *
 * Создаем архив и возвращаем дескриптор
 * 
 * Возвращает:
 * -1: Не был создан архив
 *  Значение дескриптора архива
 */
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
                        perror("Ошибка создания архива");
                        exit(EXIT_FAILURE);
                }
        }
        arch_fd = creat(arch_name, DIR_MODE);
        if(arch_fd == -1) {
                perror("Ошибка создания архива");
                exit(EXIT_FAILURE);
        }
        return arch_fd;
}

static loff_t copy_file_range(int fd_in, loff_t *off_in,
                              int fd_out, loff_t *off_out,
                              size_t len, unsigned int flags)
{
    return syscall(__NR_copy_file_range, fd_in,
                   off_in, fd_out, off_out, len, flags);
}

/*
 * end_of_arch() - конец работы с архивом
 * @arch_fd:    Дескриптор архива
 *
 * Записываем конечный нулевой header и закрываем архив
 *
 * Возвращает:
 * -1: Ошибка записи
 *  0: Успех
 */
int end_of_arch(int arch_fd) {
        struct meta_data header = {0};
        if (write(arch_fd, &header, sizeof(struct meta_data)) == -1) {
                perror("Ошибка записи");
                return -1;
        }
        close(arch_fd);
        return 0;
}

/*
 * write_to_arch() - обработка файла для записи
 * @arch_fd:    Дескриптор архива
 * @name:       Имя файла
 *
 * Обрабатываем файл для архивации
 * проверяем файл на наличие, читаем метаданные
 *
 * если файл является папкой:
 * дополняем имя файла "/" (если не было указано),
 * и передаем имя файла в write_dir_to_arch
 *
 * если файл не является папкой:
 * передаем имя файла в write_file_to_arch
 *
 * Возвращает:
 * -1: Ошибка открытия файла
 *  0: Успех
 */
int write_to_arch(int arch_fd, char *name) {
        struct stat fstat;
        stat(name, &fstat);
        if (access(name, F_OK) == -1) {
                perror("Ошибка открытия файла");
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

/*
 * write_file_to_arch() - запись файла в архив
 * @arch_fd:            дескриптор архива
 * @file:               имя файла
 *
 * Читаем метаданные файла (имя, размер, тип файла) и сохраняем в header
 * записываем header в архив, копируем содержимое файла в архив (header.size бит)
 *
 * Возвращает:
 * -1: Ошибка записи
 * -2: Ошибка чтения файла
 * -3: Ошибка копирования файла в архив
 *  0: Успех
 */
int write_file_to_arch(int arch_fd, char *file) {
        struct meta_data header;
        struct stat fstat;
        stat(file, &fstat);
        /* формируем хэдер */
        strcpy(header.name, file);
        header.size = fstat.st_size;
        header.mode = fstat.st_mode;
        /* записываем хэдер */
        if (write(arch_fd, &header, sizeof(struct meta_data)) == -1) {
                perror("Ошибка записи");
                return -1;
        }
        /*
         * открываем файл, который будем архивировать
         */
        int fd = open(file, O_RDONLY);
        if (fd == -1) {
                perror("Ошибка открытия файла");
                if (lseek(arch_fd, -sizeof(struct meta_data), SEEK_CUR) == -1) {
                        perror("Ошибка чтения архива");
                        exit(EXIT_FAILURE);
                }
                return -2;
        }
         /* копируем содержимое файла в архив */
        if (copy_file_range(fd, NULL, arch_fd, NULL, fstat.st_size, 0) == -1) {
                perror("Ошибка копирования файла");
                exit(EXIT_FAILURE);
        }

        close(fd);
        return 0;
}

/**
 * в selector фильтруем ненужные директории (текущая и наддиректория)
 */
static int selector(const struct dirent *entry) {
        if (strcmp(entry->d_name, ".") == 0) return 0;
        if (strcmp(entry->d_name, "..") == 0) return 0;
        return 1;
}

/*
 * write_dir_to_arch() - запись директории в архив
 * @arch_fd:            дескриптор архива
 * @dir:                имя директории
 *
 * Читаем метаданные директории (имя, тип файла, размер = 0) и записываем в header
 * записываем header в архив, проходим по файлам данной директории
 * и рекурсивно их обрабатываем
 *
 *
 * Возвращает:
 * -1: Ошибка записи
 * -2: Ошика открытия директории
 *  0: Успех
 */
int write_dir_to_arch(int arch_fd, char *dir) {
        struct meta_data header = {0};
        struct stat dstat;
        /* формируем хэдер */
        stat(dir, &dstat);
        strcpy(header.name, dir);
        header.mode = dstat.st_mode;
        /* записываем хэдер */
        if (write(arch_fd, &header, sizeof(struct meta_data)) == -1) {
                perror("Ошибка записи");
                return -1;
        }
        /* получаем список элеметов уровня dir */
        struct dirent **eps;
        int n = scandir(dir, &eps, selector, alphasort);
        if (n >= 0) {
                int cnt;
                for (cnt = 0; cnt < n; ++cnt) {
                        char path[PATH_MAX];
                        strcpy(path, dir);
                        /* eps[cnt]-> d_name текущий элемент */
                        strcat(path, eps[cnt]->d_name);
                        /*
                         * обрабатываем новый элемент
                         * (может быть как файлом так и папкой)
                         */
                        write_to_arch(arch_fd, path);
                }
        } else {
                perror("Ошибка открытия директории");
                return -2;
                }

        return 0;
}

/*
 * extract_from_arch() - обработка файла для извлечения
 * @arch_name:          дескриптор архива
 *
 * Проверяем на существование архива, открываем его
 * последовательно читаем метаданные (header),
 *
 * если файл - директория:
 * передаем дескриптор архива и имя директории в  extract_dir
 *
 * если файл - не директория:
 * передаем имя файла и header в  extract_file
 *
 * Возвращает:
 * -1: Ошибка существования файла
 * -2: Ошибка открытия файла
 *  0: Успех
 */
int extract_from_arch(char *arch_name) {
        int arch_fd;
        /* проверка на существование архива */
        if (access(arch_name, F_OK) == -1) {
                perror("Ошибка существования файла");
                return -1;
        }
        arch_fd = open(arch_name, O_RDONLY);
        if (arch_fd == -1) {
                perror("Ошибка открытия файла");
                return -2;
        }

        struct meta_data header;
        ssize_t len = read(arch_fd, &header, sizeof(struct meta_data));
        /*
         * обрабатываем файлы
         */
        while (len > 0) {
                /* проверка на конец архива (конечный header) */
                if (header.size == 0 && strlen(header.name) == 0) break;
                printf("Извлечение: %s\n", header.name);
                if (S_ISDIR(header.mode)) {
                        extract_dir(arch_fd, header.name);
                } else {
                        extract_file(arch_fd, header);
                }
                len = read(arch_fd, &header, sizeof(struct meta_data));
        }

        close(arch_fd);
        return 0;
}

/*
 * extract_dir() - извлечение директории
 * @arch_fd:    дескриптор архива
 * @name:       имя директории
 *
 * Проверяем существование папки, создаем папку
 *
 * Возвращает:
 * -1: Ошибка создания папки
 *  0: Успех
 */
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

/**
 * extract_file() - извлечение файла (не директории)
 * @arch_fd:    дескриптор архива
 * @header:     метаданные файла
 *
 * Проверяем существование файла, спрашиваем о перезаписи
 * создаем файл, копируем в него данные из архива, согласно header
 * (в данном случае size бит данных)
 *
 * Возвращает:
 * -1: Отказ пользователя в перезаписи
 *  0: Успех
 */
int extract_file(int arch_fd, struct meta_data header) {
        char *name = header.name;
        off_t size = header.size;
        int fd;
        /* если файл существует */
        if (access(name, F_OK) == 0) {
                int c;
                printf("Файл существует, перезаписать? [y/n]: ");
                c = toupper(getchar());
                int answ = c;
                while (c != '\n' && c != EOF) c = getchar();
                if (answ != 'Y') {
                        if (lseek(arch_fd, size, SEEK_CUR) == -1) {
                                perror("Ошибка чтения архива");
                                /*аварийное прерывание тк,
                                 *последущая разархивация невозможна
                                 */
                                abort();
                        }
                        return -1;
                }
        }
        /* создаем или перезаписываем файл */
        fd = creat(name, DIR_MODE);

        if (copy_file_range(arch_fd, NULL, fd, NULL, size, 0) == -1) {
            perror("Ошибка копирования файла");
            exit(EXIT_FAILURE);
        }

        close(fd);
        return 0;
}
