# import socket
# import numpy as np
import time, sys
from multiprocessing import shared_memory
# from memory_profiler import profile
# import tracemalloc

message = ''
mem_size = 0

if sys.argv[1] == "1B":
    mem_size = 1
elif sys.argv[1] == "100B":
    mem_size = 100
elif sys.argv[1] == "1000B":
    mem_size = 1000
elif sys.argv[1] == "10000B":
    mem_size = 10000
elif sys.argv[1] == "100000B":
    mem_size = 100000
elif sys.argv[1] == "1000000B":
    mem_size = 1000000
else:
    print("Message size incorrect.")
    exit(1)

for i in range (0, mem_size - 1):
    message = message + 'a'

# mem_size = np.dtype(NP_DATA_TYPE).itemsize * np.prod(data.shape)

ts_start = time.time() * 10**9
shm = shared_memory.SharedMemory(name="shared_memory", create=True, size=mem_size)
ts_end = time.time() * 10**9
print("[QLOG] shm_open + mmap latency: {} ns".format(ts_end - ts_start))

# message = message.encode()
ts_start = time.time() * 10**9
res = bytes(message, 'utf-8')
ts_end = time.time() * 10**9
print("[QLOG] serialization latency: {} ns".format(ts_end - ts_start))

ts_start = time.time() * 10**9
shm.buf[:len(message)] = res
ts_end = time.time() * 10**9
print("[QLOG] write shared memory latency: {} ns".format(ts_end - ts_start))

# shm.unlink()
time.sleep(10)