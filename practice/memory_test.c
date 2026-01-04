#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MEGABYTE (1024 * 1024)

int main() {
    void *myblock = NULL;
    int count = 0;

    while (1) {
        myblock = (void*) malloc(MEGABYTE);
        if (myblock == NULL) {
            printf("Memory allocation failed after %d MB\n", count);
            break;
        }
        printf("Allocated %d MB\n", (++count)*MEGABYTE);
        memset(myblock, 1, MEGABYTE); 
        sleep(1);

        if (count == 10) {
            break;
        }
    }

    sleep(600);
}