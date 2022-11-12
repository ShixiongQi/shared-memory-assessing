#include <stdio.h>
#include <stdlib.h> // for exit
#include <string.h> // for strncpy
#include <sys/shm.h> // for shm_ calls

#include "shm_common.h"
#include "timer.h"

int main(int argc, char **argv) {
    int shm_id = -1;
    void *shm_addr = NULL;
    struct timespec t_start;
    struct timespec t_end;

    if (argc != 2) {
        printf("Usage: ./shm_write <message length>\n");
        printf("\t ./shm_write 100000B // 100000B length message\n");
        exit(1);
    }

    // char *msg = argv[1];
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
    
    /*
     * Get a key from a dummy file.
     * The alternative way is to use our own key, but using
     * a file makes it easier to share with other programs.
     */
    get_monotonic_time(&t_start);
    key_t shm_key = ftok(SHMKEY_FILE, 0);
    if (shm_key == -1) {
        perror("ftok failed");
        exit(1);
    }
    get_monotonic_time(&t_end);
    printf("[QLOG] ftok latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    /*
     * Creating the shared memory only from here.
     * Reader and Cleaner would fail if shared memory with the
     * key pointed by SHMKEY_FILE doesn't exist.
     */
    get_monotonic_time(&t_start);
    shm_id = shmget(shm_key, SHMEM_SIZE, IPC_CREAT | SHMEM_FLAGS);
    if (shm_id == SHM_ERROR) {
        perror("shmget failed");
        exit(1);
    }
    get_monotonic_time(&t_end);
    printf("[QLOG] shmget latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    printf("shm_key=%u shm_id=%u\n", shm_key, shm_id);

    // if no flag is specified to shmat call, it will preserve
    // the permissions from the shmem.
    // shmat returns -1 on failure.
    get_monotonic_time(&t_start);
    shm_addr = shmat(shm_id, NULL, 0);
    if (shm_addr == (char *)SHM_ERROR) {
        perror("Failed to attach the memory");
        // Dont forget to remove the shm_id in case of failure.
        shmctl(shm_id, IPC_RMID, NULL);
        exit(1);
    }
    get_monotonic_time(&t_end);
    printf("[QLOG] shmat latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    printf("Writing to shmem: shm_addr=%p length of message=%ld Bytes\n", shm_addr, strlen(message));

    get_monotonic_time(&t_start);
    // write the given message to the shared memory
    strncpy((char *)shm_addr, message, strlen(message));
    get_monotonic_time(&t_end);
    printf("[QLOG] write shared memory latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    // detach the mapped memory
    shmdt(shm_addr);

    // not removing the shmem from here.
    // it will be cleaned up separately from another program
    return 0;
}
