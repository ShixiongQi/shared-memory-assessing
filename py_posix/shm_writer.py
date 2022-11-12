import time, sys
import mmap
import hashlib

# 3rd party modules
import posix_ipc
import utils

message = ''
length = 0

if sys.argv[1] == "1B":
    length = 1
elif sys.argv[1] == "100B":
    length = 100
elif sys.argv[1] == "1000B":
    length = 1000
elif sys.argv[1] == "10000B":
    length = 10000
elif sys.argv[1] == "100000B":
    length = 100000
elif sys.argv[1] == "1000000B":
    length = 1000000
else:
    print("Message length incorrect.")
    exit(1)

for i in range (0, length - 1):
    message = message + 'a'

params = utils.read_params()
SHM_SIZE = params["PAGE_SIZE"] * params["NUM_PAGES"]

ts_start = time.time() * 10**6
memory = posix_ipc.SharedMemory(params["SHARED_MEMORY_NAME"], posix_ipc.O_CREX, size=SHM_SIZE)
ts_end = time.time() * 10**6
print("[QLOG] shm_open latency: {} us".format(ts_end - ts_start))

ts_start = time.time() * 10**6
mapfile = mmap.mmap(memory.fd, memory.size)
ts_end = time.time() * 10**6
print("[QLOG] mmap latency: {} us".format(ts_end - ts_start))

# Once I've mmapped the file descriptor, I can close it without
# interfering with the mmap.
memory.close_fd()

print("Writing to shmem: name={}, msg length={}, shm_addr={}".format(params["SHARED_MEMORY_NAME"], len(message), mapfile))

ts_start = time.time() * 10**6
# Writing to shmem
utils.write_to_memory(mapfile, message)
ts_end = time.time() * 10**6
print("[QLOG] write shared memory latency: {} us".format(ts_end - ts_start))

# Dettach shared memory
mapfile.close()
# posix_ipc.unlink_shared_memory(params["SHARED_MEMORY_NAME"])