#!/bin/bash

# Check if a file is provided as an argument
if [ $# -eq 0 ]; then
    echo "Usage: $0 <file>"
    exit 1
fi

# Get the filename from the first argument
file="$1"

# Check if the file exists
if [ ! -f "$file" ]; then
    echo "File $file not found!"
    exit 1
fi

# Check if the file is readable
if [ ! -r "$file" ]; then
    echo "File $file is not accessible!"
    exit 1
fi

# Proceed with the script if the file exists and is accessible

chunk_size=10000
file_size=$(wc -c < "$file")

for ((i=0; i<file_size; i+=chunk_size)); do
    end=$((i+chunk_size))
    chunk=$(dd if="$file" bs=1 skip="$i" count="$chunk_size" 2>/dev/null)

    # printf "%x\r\n%s\n\n\n\n\n\n" "${#chunk}" "$chunk" # Print chunk size in hexadecimal and chunk data
done

# Print the final chunk indicating the end
# printf "\r\n0\r\n\r\n"

curl -X POST -H "Transfer-Encoding: chunked; filename=\"$file\"" --data-binary "@$file" localhost:8080
