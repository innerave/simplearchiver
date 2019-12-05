#include "archiver.h"

int main(int argc, char *argv[])
{
    int opt;

    while((opt = getopt(argc, argv, ":e:c:a:")) != -1) {
        switch(opt) {
        case 'e':
            printf("Извлечение из архива : %s\n", optarg);
            break;
        case 'c':
            printf("Добавление в архив: %s\n", optarg);
            break;
        case ':':
            printf("Требуется имя архива\n");
            break;
        case '?':
            printf("Неизвестная команда: %c\n", optopt);
            break;
        }
    }
    for(; optind < argc; optind++)
        printf("Выбран файл: %s\n", argv[optind]);
    return 0;
}