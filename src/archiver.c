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
                fprintf("Невозможно создать директорию %s: %s", path, strerror(return_code));
            }

            *p = '/';
        }
    }

    if (mkdir(path, DIR_MODE) < 0){
        fprintf("Невозможно создать директорию %s: %s", path, strerror(return_code));
    }

    free(path);
    return 0;
}
