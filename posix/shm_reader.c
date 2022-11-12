#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "shm_common.h"
#include "timer.h"

int main(void) {

    void *shm_addr = NULL;
    int shm_fd = -1;
    struct timespec t_start;
    struct timespec t_end;

    get_monotonic_time(&t_start);
    shm_fd = shm_open(SHM_NAME, O_RDONLY, SHM_MODE);
    if (shm_fd == -1) {
        perror("shm_open failed");
        return EXIT_FAILURE;
    }
    get_monotonic_time(&t_end);
    printf("[QLOG] shm_open latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    get_monotonic_time(&t_start);
    shm_addr = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_addr == MAP_FAILED) {
        perror("mmap with shm_fd failed");
        return EXIT_FAILURE;
    }
    get_monotonic_time(&t_end);
    printf("[QLOG] mmap latency: %ld\n", get_elapsed_time_nano(&t_start, &t_end));

    printf("Reading from addr=%p, msg length=%ld\n", shm_addr, strlen((char *)shm_addr));

    // getting some stats about the shared memory just accessed.
    struct stat shmstat;
    fstat(shm_fd, &shmstat);

#ifdef __linux__
    printf("shm_stats: st_size=%ld, st_blocks=%ld, st_blksize=%ld\n",
            shmstat.st_size,
            shmstat.st_blocks,
            shmstat.st_blksize);
#elif defined(__MACH__) || defined(__APPLE__)
    /*
     * macOS doesn't provide the file system interface to access
     * shared memory segments. So the blocks and blksize fields
     * are not populated in the stat results.
     */
    printf("shm_stats: st_size=%lld\n", shmstat.st_size);
#endif

    printf("Cleaning up %s segment\n", SHM_NAME);
    // do the necessary cleanup.
    munmap(shm_addr, SHM_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);

    return EXIT_SUCCESS;
}
