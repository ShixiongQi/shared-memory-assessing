# !/bin/bash
# Automatically assess POSIX (Python) shared memory

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