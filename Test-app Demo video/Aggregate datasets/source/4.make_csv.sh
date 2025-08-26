#!/bin/bash

LOG_FILE="output.txt"
OUTPUT_CSV="output.csv"

TAGS=$(grep -oP "\['.*?'\]" "$LOG_FILE" \
    | tr -d "[]'" \
    | tr ' ' '\n' \
    | grep -v '^$' \
    | sort -u \
    | uniq \
    | tr '\n' ',' \
    | sed 's/,$//')

echo "app name,${TAGS}" > "$OUTPUT_CSV"

while read -r line; do
    app_package=$(echo "$line" | awk '{print $1}')
    tags_in_line=$(echo "$line" | grep -oP "\['.*?'\]" | tr -d "[]'" | tr ' ' '\n' | grep -v '^$' | sort -u | xargs)

    row="$app_package"
    for tag in $(echo "$TAGS" | tr ',' ' '); do
        if [[ " $tags_in_line " =~ " $tag " ]]; then
            row="$row,o"
        else
            row="$row,X"
        fi
    done
    echo "$row" >> "$OUTPUT_CSV"
done < "$LOG_FILE"

echo "CSV file $OUTPUT_CSV is generated."

