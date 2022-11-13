import os, time
from multiprocessing.shared_memory import SharedMemory

# Connect to shared memory object
ts_start = time.time() * 10**9
shm = SharedMemory(name="shared_memory")
ts_end = time.time() * 10**9
print("[QLOG] shm_open + mmap latency: {} ns".format(ts_end - ts_start))

ts_start = time.time() * 10**9
res = shm.buf.decode("utf-8")
ts_end = time.time() * 10**9
print("[QLOG] deserialization latency: {} ns".format(ts_end - ts_start))

print("Read message: {}".format(res))

shm.close()
shm.unlink()