# !/bin/bash
# Automatically assess System-V shared memory

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