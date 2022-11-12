#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h> // for sleep
#include <string.h>

#include "shm_common.h"
#include "timer.h"

int main(void) {

    int shm_id = -1;
    char *shm_addr;
    key_t shm_key = -1;
    struct timespec t_start;
    struct timespec t_end;

    get_monotonic_time(&t_start);
    shm_key = ftok(SHMKEY_FILE, 0);

    if (shm_key == -1) {
        perror("ftok failed");
        exit(1);
    }
    get_monotonic_time(&t_end);
    printf("[QLOG] ftok latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    /*
     * If a segment with shm_key exists already, then shmget
     * returns the id of the existing segment. Size option is
     * ignored in that case.
     * If a segment with shm_key doesn't exist, and if IPC_CREAT
     * flag is passed, then a new segment will be created with
     * with the given size.
     */
    get_monotonic_time(&t_start);
    shm_id = shmget(shm_key, SHMEM_SIZE, SHMEM_FLAGS);
    if (shm_id == SHM_ERROR) {
        perror("shmget failed");
        exit(1);
    }
    get_monotonic_time(&t_end);
    printf("[QLOG] shmget latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));


    printf("shm_key=%u shm_id=%d\n", shm_key, shm_id);

    /*
     * if we don't specify the address to attach the shared memory,
     * virtual memory address returned from shmat after each invocation
     * will be different.
     * The shm_id will be the same for a given key.
     */
    get_monotonic_time(&t_start);
    shm_addr = shmat(shm_id, NULL, 0);
    if (shm_addr == (char *)SHM_ERROR) {
        perror("shmat failed");
        exit(1);
    }
    get_monotonic_time(&t_end);
    printf("[QLOG] shmat latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    // reading from shared memory
    printf("Reading from shmem: shm_addr=%p msg length=%ld\n", shm_addr, strlen(shm_addr));

    // detach the mapped memory
    shmdt(shm_addr);

    return 0;
}
