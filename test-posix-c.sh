# !/bin/bash
# Automatically assess POSIX (C) shared memory

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