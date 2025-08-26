#!/bin/bash

SOURCE_DIR="/..."
OUTPUT_DIR="/..."

mkdir -p "$OUTPUT_DIR"

THREADS=16

index=1

for apk_file in "$SOURCE_DIR"/*.apk; do
    base_name=$(basename "$apk_file" .apk)
    
    output_subdir="$OUTPUT_DIR/$base_name"
    mkdir -p "$output_subdir"

    echo "Processing $apk_file with 7z..."

    7z x -y -mmt="$THREADS" "$apk_file" -o"$output_subdir" -ir!*.apk -ir!*.so -ir!*.dex > /dev/null 2>&1
    
    if [ $? -ne 0 ]; then
        echo "[$index] Failed to extract $apk_file."
    else
        echo "[$index] Extracted $apk_file to $output_subdir."
    fi

    index=$((index + 1))
done

echo "All APK files processed and extracted to $OUTPUT_DIR."

