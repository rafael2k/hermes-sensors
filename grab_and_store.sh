#!/bin/bash

OUTPUT_FILE="${1:-output.csv}"

while true; do
    gpspipe -w -n 10 | grep -m 1 TPV | jq -j '[.time, .lat, .lon] | @csv' >> ${OUTPUT_FILE}
    echo >> ${OUTPUT_FILE}
done
