# !/bin/bash
# Automatically assess different shared memory implementation

echo "Assessing POSIX shared memory (native C)"
    pushd posix
    for i in {0..9}
    do
        for MSG_SIZE in 1B 100B 1000B 10000B 100000B 1000000B
        do
            ./shm_writer.exe ${MSG_SIZE} && ./shm_reader.exe && sleep 0.1
        done
    done
    popd

echo "Assessing System-V shared memory (native C)"
    pushd sysv
    for i in {0..9}
    do
        for MSG_SIZE in 1B 100B 1000B 10000B 100000B 1000000B
        do
            ./shm_writer.exe ${MSG_SIZE} && ./shm_reader.exe && ./shm_clean.exe && sleep 0.1
        done
    done
    popd

echo "Assessing POSIX shared memory (Python)"
    pushd py_posix
    for i in {0..9}
    do
        for MSG_SIZE in 1B 100B 1000B 10000B 100000B 1000000B
        do
            python3 shm_writer.py ${MSG_SIZE} && python3 shm_reader.py && sleep 0.1
        done
    done
    popd