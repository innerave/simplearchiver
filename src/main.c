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
    while((opt = getopt(argc, argv, ":e:c:h")) != -1) {
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
        case 'h':
            mode=HELP;
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
        if (mode==HELP) {
            printf("Использование:\n");
            printf("./arch -e {имя_архива}                                извлечение из архива\n");
            printf("./arch -c {имя_архива} {файл1} {файл2} {файл3} ...    создание архива\n");
            return 0;
        }
        if (mode==CREATE){
            if (optind>=argc) {
                printf("Требуется выбрать файлы для добавления в архив\n");
                return 2;
            }
            int arch_fd=create_arch(arch);
            if (arch_fd==-1) return 3;
            for(int optind_=optind; optind_ < argc; optind_++) {
                printf("Запись файла: %s\n", argv[optind_]);
                write_to_arch(arch_fd, argv[optind_]);
            } 
            end_of_arch(arch_fd);
        }
        if (mode==EXTRACT)  {
            extract_from_arch(arch);
        }
    }
    return 0;
}