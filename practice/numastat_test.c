#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MEGABYTE (1024 * 1024)

int main() {
    void *myblock = NULL;
    int count = 0;

    while (1) {
        myblock = (void *) malloc(MEGABYTE);

        if (!myblock) {
            printf("Allocated %d MB before failure.\n", count);
            break;
        }

        printf("Allocated %d MB\n", (++count) * MEGABYTE);
        memset(myblock, 0, MEGABYTE);

        sleep(1);
    }
}