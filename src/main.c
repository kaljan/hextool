/**
 * @file    main.c
 * @author  Nikolai Naurotski (kaljan.nothern@gmail.com)
 * @version 1.0.0
 * @date    20-01-24
 *
 * @brief
 */
#include "tools.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

int hexdump_file(const char* file_name, int flags) {
    ASSERT_STRING_RET(file_name, -1)
    int file_des = -1;
    struct stat file_stat;
    uint8_t* file_ptr = NULL;

    if (stat(file_name, &file_stat) < 0) {
        msg_errno("open file \"%s\" failed", file_name);
        return 1;
    }
    tools_print_stat(&file_stat);
    print_d("OPEN FILE: %s", file_name);
    file_des = open(file_name, O_RDONLY);
    if (file_des < 0) {
        msg_errno("open file \"%s\" failed", file_name);
        return 1;
    }

    file_ptr = mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, file_des, 0);
    if (NULL == file_ptr) {
        msg_errno("open file \"%s\" failed", file_name);
        close(file_des);
        return 1;
    }

    tools_hexdump(file_ptr, file_stat.st_size, 0);

    munmap(file_ptr, file_stat.st_size);
    close(file_des);

    return 0;
}

int main(int argc, char** argv) {
    if ((argc < 2) || (argv == NULL) || *(argv + 1) == NULL) {
        print_e("bad arg");
        return 1;
    }
    return hexdump_file(*(argv + 1), 0);
}
