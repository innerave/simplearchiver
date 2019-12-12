#include "archiver.h"

int main(int argc, char *argv[])
{
    int opt;

    while((opt = getopt(argc, argv, ":e:c:")) != -1) {
        switch(opt) {
        case 'e':
            printf("Извлечение из архива : %s\n", optarg);
            take_meta(optarg);
            break;
        case 'c':
            printf("Добавление в архив: %s\n", optarg);
            make_meta(optarg,argv,optind,argc);
            break;
        case ':':
            printf("Требуется имя архива\n");
            break;
        case '?':
            printf("Неизвестная команда: %c\n", optopt);
            break;
        }
    }
    return 0;
}