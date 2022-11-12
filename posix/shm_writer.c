#include <fcntl.h> // for flags O_CREAT etc..
#include <stdio.h>
#include <stdlib.h> // for EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>
#include <sys/mman.h> // for mmap, shm_open
#include <unistd.h> // for close()

#include "shm_common.h"
#include "timer.h"

int main(int argc, char **argv) {

    int fd = -1;
    struct timespec t_start;
    struct timespec t_end;

    if (argc != 2) {
        printf("Usage: ./shm_write <message>\n");
        return EXIT_FAILURE;
    }

    char* message;
    size_t length;

    char one[1] = {0};
    char hundred[100] = {0};
    char thousand[1000] = {0};
    char ten_thousand[10000] = {0};
    char hundred_thousand[100000] = {0};
    char million[1000000] = {0};

    if (strcmp(argv[1], "1B") == 0) {
        message = one;
        length = 1;
    } else if (strcmp(argv[1], "100B") == 0) {
        message = hundred;
        length = 100;
    } else if (strcmp(argv[1], "1000B") == 0) {
        message = thousand;
        length = 1000;
    } else if (strcmp(argv[1], "10000B") == 0) {
        message = ten_thousand;
        length = 10000;
    } else if (strcmp(argv[1], "100000B") == 0) {
        message = hundred_thousand;
        length = 100000;
    } else if (strcmp(argv[1], "1000000B") == 0) {
        message = million;
        length = 1000000;
    } else {
        printf("Message length incorrect.\n");
        exit(1);
    }

    for (int i = 0; i < length - 1; i++)
        message[i] = 'a';

    message[length - 1] = '\0';

    get_monotonic_time(&t_start);
    fd = shm_open(SHM_NAME, SHM_FLAGS, SHM_MODE);
    if (fd == -1) {
        perror("shm_open failed");
        return EXIT_FAILURE;
    }
    get_monotonic_time(&t_end);
    printf("[QLOG] shm_open latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    get_monotonic_time(&t_start);
    // extend the shmem size
    ftruncate(fd, SHM_SIZE);
    get_monotonic_time(&t_end);
    printf("[QLOG] ftruncate latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    get_monotonic_time(&t_start);
    // map some memory to shmem segment to write to.
    void *shm_addr = NULL;
    shm_addr = mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_addr == MAP_FAILED) {
        perror("mmap failed");
        return EXIT_FAILURE;
    }
    get_monotonic_time(&t_end);
    printf("[QLOG] mmap latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    // char *msg = argv[1];
    printf("Writing to shmem: name=%s, msg length=%ld, shm_addr=%p\n",
            SHM_NAME, strlen(message), shm_addr);
    get_monotonic_time(&t_start);
    strncpy((char *)shm_addr, message, strlen(message));
    get_monotonic_time(&t_end);
    printf("[QLOG] write shared memory latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    // unmap the memory and close the file descriptor
    munmap(shm_addr, SHM_SIZE);
    close(fd);

    return EXIT_SUCCESS;
}
