#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define MEGABYTE 1024 * 1024

int main() {
    int output_fd;
    char message[MEGABYTE] = "";
    char file_name[] = "./test.dump";

    int count = 0;

    output_fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC);

    for (; ;) {
        count++;
        write(output_fd, message, MEGABYTE);
        printf("Write File - Current Size : %d KB\n", count * 1024);
        sleep(1);
    }

    return 0;
}