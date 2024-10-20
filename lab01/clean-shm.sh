#/bin/bash

shm_files=$(ls /dev/shm)
# if /dev/shm is empty, then exit
if [ -z "$shm_files" ]; then
    echo "No files in /dev/shm"
    exit 0
fi

# remove all files in /dev/shm
echo "Removing $shm_files in /dev/shm"
for file in $shm_files; do
    rm -f /dev/shm/$file
done