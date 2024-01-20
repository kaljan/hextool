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
#include <sys/ioctl.h>

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

static void strhex(char* str, size_t str_size, const uint8_t* data, size_t size) {
    size_t cursor = 0;
    *str = 0;
    for (size_t i = 0; i < size; i++) {
        if (cursor >= str_size) {
            break;
        }
        cursor = strlen(str);
        if (i != 0) {
            if ((i % 16) == 0) {
                snprintf(&str[cursor], str_size - cursor, "\n");
            } else if ((i % 8) == 0) {
                snprintf(&str[cursor], str_size - cursor, "  ");
            } else {
                snprintf(&str[cursor], str_size - cursor, " ");
            }
        }
        cursor = strlen(str);
        snprintf(&str[cursor], str_size - cursor, "%02X", data[i]);
    }
}

int hexdump_stdio(int flags) {
    uint8_t buf[16];
    char hex_str[50];
    int sio_fd = fileno(stdin);
    ssize_t ret = 0;
    uint32_t index = 0;
    while ((ret = read(sio_fd, buf, 16)) > 0) {
        strhex(hex_str, 50, buf, ret);
        // tools_strhex(hex_str, 25, buf, 8, 0, 0, " ");
        printf("%08X: %s\n", index, hex_str);
        index += 8;
    }

    return 0;
}


int main(int argc, char** argv) {
    if ((argc < 2) || (argv == NULL) || *(argv + 1) == NULL) {
        hexdump_stdio(0);
        // print_e("bad arg");
        return 0;
    }

    return hexdump_file(*(argv + 1), 0);
}

#if 0
static void print_stdin(void) {
    struct stat stdin_stat;
    fstat(fileno(stdin), &stdin_stat);
    tools_print_stat(&stdin_stat);
    int bytes_avail = 0;
    if (ioctl(fileno(stdin), FIONREAD, &bytes_avail) < 0) {
        msg_errno("ioctl(FIONREAD) failed");
        return;
    }

    print_i("bytes avail: %d", bytes_avail);

    int c = 0;
    printf("\e[1;35mpipe contents\e[37m:\e[0m ");
    while((c = getchar()) != EOF) {
        putchar(c);
    }
}
#endif
