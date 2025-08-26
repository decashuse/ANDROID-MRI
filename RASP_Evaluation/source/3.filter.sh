#!/bin/bash

LOG_FILE="log.txt"
OUTPUT_FILE="output.txt"

> "$OUTPUT_FILE"

awk '
/^File:/ {
    # 1. "File: " 
    sub(/^File: /, "")

    split($1, parts, "/")
    key = parts[1]

    for (i = 2; i <= NF; i++) {
        tags[key] = tags[key] " " $i
    }
}
END {
    for (key in tags) {
        split(tags[key], tag_array, " ")
        asort(tag_array)
        unique_tags = ""
        for (i = 1; i <= length(tag_array); i++) {
            if (i == 1 || tag_array[i] != tag_array[i-1]) {
                unique_tags = unique_tags " " tag_array[i]
            }
        }
        print key, unique_tags
    }
}' "$LOG_FILE" > "$OUTPUT_FILE"

cat "$OUTPUT_FILE"

