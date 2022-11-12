import mmap
import os
import sys
import hashlib

# 3rd party modules
import posix_ipc
import utils

params = utils.read_params()

# Writer has already created shared memory.
memory = posix_ipc.SharedMemory(params["SHARED_MEMORY_NAME"])

# MMap the shared memory
mapfile = mmap.mmap(memory.fd, memory.size)

# Once I've mmapped the file descriptor, I can close it without
# interfering with the mmap. This also demonstrates that os.close() is a
# perfectly legitimate alternative to the SharedMemory's close_fd() method.
os.close(memory.fd)

utils.read_from_memory(mapfile)

# Destroying shared memory
mapfile.close()
posix_ipc.unlink_shared_memory(params["SHARED_MEMORY_NAME"])