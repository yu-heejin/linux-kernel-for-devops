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
        if (count % 1000 == 0) {
            output_fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC);
        }
        // printf("Write File - Current Size : %d KB\n", count * 1024);

        if (count >= 5000) {
            break;
        }

        // sleep(1);
    }

    return 0;
}