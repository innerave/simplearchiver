#include "archiver.h"

int main(int argc, char *argv[])
{
    int opt;
    /*  optarg – указатель на текущий аргумент, если таковой имеется.
        optind – индекс на следующий указатель argv, который будет обработан при следующем вызове getopt().
        optopt – нераспознанная опция.
        argv[optind] – и далее – собранные опции*/
    while((opt = getopt(argc, argv, ":e:c:")) != -1) {
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
    return 0;
}