#!/bin/bash

OUTPUT_FILE="${1:-output.csv}"

echo "timestamp, latitude, longitude, Vbatt, Vcurr, SOC" > ${OUTPUT_FILE}

while true; do
    gpspipe -w -n 10 | grep -m 1 TPV | jq -j '[.time, .lat, .lon] | @csv' >> ${OUTPUT_FILE}
    echo -n "," >> ${OUTPUT_FILE}
    ./get_battery_data.py >> ${OUTPUT_FILE}
    echo >> ${OUTPUT_FILE}
done
