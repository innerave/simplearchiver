#include "archiver.h"

int main(int argc, char *argv[])
{
    int opt;
    int mode=-1;
    char* arch=NULL;
    /*  optarg – указатель на текущий аргумент, если таковой имеется.
        optind – индекс на следующий указатель argv, который будет обработан при следующем вызове getopt().
        optopt – нераспознанная опция.
        argv[optind] – и далее – собранные файлы*/
    while((opt = getopt(argc, argv, ":e:c:")) != -1) {
        switch(opt) {
        case 'e':
            printf("Извлечение из архива : %s\n", optarg);
            arch=optarg;
            mode=EXTRACT;
            break;
        case 'c':
            printf("Добавление в архив: %s\n", optarg);
            arch=optarg;
            mode=CREATE;
            break;
        case ':':
            printf("Требуется имя архива\n");
            break;
        case '?':
            printf("Неизвестная команда: %c\n", optopt);
            break;
        default:
            abort();
        }
        if (mode==-1) return 1;
        if (mode==CREATE){
            create_arch(arch);
            if (optind>=argc) {
                printf("Требуется выбрать файлы для добавления в архив\n");
                return 2;
            }
            for(; optind < argc; optind++){
                printf("Выбранные файлы: %s\n", argv[optind]);
            }
        }
        if (mode==EXTRACT)  {
            /* code */
        }
    }
    return 0;
}