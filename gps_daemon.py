#!/usr/bin/python3
import os
import datetime
import time
from gps3.agps3threaded import AGPS3mechanism

delay = 1 # delay between each sampling
time_to_create_dump = 3600 # time in seconds between each report

path="/var/spool/gps/"

agps_thread = AGPS3mechanism()  # Instantiate AGPS3 Mechanisms
agps_thread.stream_data()  # From localhost (), or other hosts, by example, (host='gps.ddns.net')
agps_thread.run_thread()  # Throttle time to sleep after an empty lookup, default 0.2 second, default daemon=True

try:
    os.mkdir(path)
except OSError as error:
    print("Directory" + path + "already created. Good.")

time.sleep(1)
next_time = time.time() + delay
counter = 0

ct = datetime.datetime.now().isoformat(timespec='minutes')
path_file = os.path.join(path, ct)
fd = open(path_file,"wb")
fd.write("Time Stamp, Latitude, Latitude")

while True:

    if counter == time_to_create_dump:
        fd.close(fd)
        ct = datetime.datetime.now().isoformat(timespec='minutes')
        path_file = os.path.join(path, ct)
        fd = open(path_file,"wb")
        fd.write("Time Stamp, Latitude, Latitude")
        counter = 0

    time.sleep(max(0, next_time - time.time()))

    fd.write(agps_thread.data_stream.time + ",")
    fd.write(agps_thread.data_stream.time + ",")
    fd.write(str(agps_thread.data_stream.lat) + ",")
    fd.write(str(agps_thread.data_stream.lon) + ",")

    next_time += delay
    count += 1
# controller.get_solar_voltage
# controller.get_load_voltage
#
# controller.get_solar_current
# controller.get_load_current
