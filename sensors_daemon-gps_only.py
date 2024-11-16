#!/usr/bin/python3
import os
import datetime
import time
import configparser
import signal
import sys
import platform
import socket

import gps

emergency = False

def emergency_handler(signal, frame):
    global emergency
    emergency = True
    print("Emergency handler")

signal.signal(signal.SIGUSR1, emergency_handler)    


config = configparser.ConfigParser()
config.read('/etc/sbitx/sensors.ini')

delay = config.getint('main', 'sample_time', fallback=1) # delay between each sampling
time_to_create_dump = config.getint('main', 'time_per_file', fallback=3600)  # time in seconds between each report
email = config.get('main', 'email', fallback='admin@hermes.radio') # destination email of the data

path="/var/spool/sensors/"

try:
    os.mkdir(path)
except OSError as error:
    print("Directory " + path + " already created. Good.")

next_time = time.time() + delay
counter = 0

ct = datetime.datetime.now().isoformat(timespec='minutes')
path_file = os.path.join(path, ct + ".csv")
fd = open(path_file,"w")
fd.write("Time Stamp, Latitude, Longitude\n")

session = gps.gps(mode=gps.WATCH_ENABLE)

while  0 == session.read():

    if not (gps.MODE_SET & session.valid):
        continue

    if emergency == True:
        # write a last coordinate, the emergency one
        fd.write(datetime.datetime.now().strftime("%s") + ",")
        lat = str(session.fix.latitude)
        if lat == "n/a":
            fd.write("0,")
        else:
            fd.write(lat + ",")
        lon = str(session.fix.longitude)
        if lon == "n/a":
            fd.write("0\n")
        else:
            fd.write(lon + "\n")

        fd.close()
        cmd_string = 'enc_sensors -s -i ' +  path_file + ' -e ' + email + ' -f ' + 'root@' + platform.node() + ' &'

        print(cmd_string)
        os.system(cmd_string);

        fd = open(path_file, "a")
        emergency = False

    while next_time > time.time():
        if not (gps.MODE_SET & session.valid):
            session.read()
            continue
        time.sleep(1)

    fd.write(datetime.datetime.now().strftime("%s") + ",")

    if ((gps.isfinite(session.fix.latitude) and
         gps.isfinite(session.fix.longitude))):

        lat = str(session.fix.latitude)
        fd.write(lat + ",")

        lon = str(session.fix.longitude)
        fd.write(lon + "\n")

        #print(lat + ',' + lon)
    else:
        fd.write("0,0\n")

        # fd.flush()
        
    next_time += delay
    counter += delay

    if counter >= time_to_create_dump:
        fd.close()
        cmd_string = 'enc_sensors -i ' +  path_file + ' -e ' + email + ' -f ' + 'root@' + platform.node() + ' &'
        print(cmd_string)
        os.system(cmd_string);

        ct = datetime.datetime.now().isoformat(timespec='minutes')
        path_file = os.path.join(path, ct + ".csv")
        fd = open(path_file,"w")
        fd.write("Time Stamp, Latitude, Longitude\n")
        counter = 0
