#!/bin/bash

file="HAHAHA.png"
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
