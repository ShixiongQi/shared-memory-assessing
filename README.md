# Assessing different Shared Memory implementation

1. [POSIX interfaces (Native C)](posix)
2. [System V interfaces (Native C)](sysv)
3. [POSIX interfaces (python)](py_posix)

All the implementations were compiled and tested on Linux (Ubuntu 20.4). 

## Environment Setup
### Compile POSIX interfaces (Native C) and System V interfaces (Native C)
Run `make` inside the subfolders to build for the target system. POSIX calls on Linux requires `-lrt` to run, so [Makefile](posix/Makefile) includes `-lrt` when building on Linux.

### Install *posix_ipc* Python module 
posix_ipc is available on PyPI:
```
pip install posix-ipc
```
Or you can clone the project and install it using setup.py:
```
git clone https://github.com/osvenskan/posix_ipc.git
cd posix_ipc
python setup.py install
```
*posix_ipc* is compatible with all supported versions of Python 3. For complete documentation of *posix_ipc* module, see https://github.com/osvenskan/posix_ipc.git.

## Assessing methodologies

### [POSIX interfaces (Native C)](posix)
Run shared memory writer
```
./shm_writer.exe $MSG_SIZE
```

Run shared memory reader
```
./shm_reader.exe
```

### [System V interfaces (Native C)](sysv)
Run shared memory writer
```
./shm_writer.exe $MSG_SIZE
```

Run shared memory reader (I separate the shared memory cleaner in System-V implementation)
```
./shm_reader.exe && ./shm_clean.exe
```

### [POSIX interfaces (python)](py_posix)
Run shared memory writer
```
python3 shm_writer.py $MSG_SIZE
```

Run shared memory reader
```
python3 shm_reader.py
```

### Typical messaging flow between shared memory writer and reader

1. Writer creates a shared memory object (shm_open in POSIX, ftok + shmget in System-V)

2. Writer attaches to the shared memory object (mmap in POSIX, shmat in System-V)

3. Writer writes contents to the shared memory object

4. Writer sends the pointer to the Reader (e.g., using UNIX socket, SKMSG, RTE RING)

5. Reader attaches to shared memory object (**Note: this step can be done during the startup**)

6. Reader reads the shared memory object 

### Latencies in Shared Memory Communication
Based on the messaging flow, we can breakdown the shared memory communication latency into the following pieces:
1. Shared memory object creation latency;
2. Shared memory object attaching latency;
3. Shared memory object write latency;
4. Pointer transmission latency;

There are other latencies, which are not derived from shared memory communication, but may amortize the benefit of shared memory communication:
1. Serialization/Desrialization latency to support structured data/binary data conversion
