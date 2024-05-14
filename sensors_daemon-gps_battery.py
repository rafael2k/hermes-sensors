#!/usr/bin/python3
import os
import datetime
import time
import configparser

from gps3.agps3threaded import AGPS3mechanism
from epevermodbus.driver import EpeverChargeController

delay = config.getint('main', 'sample_time', fallback=1) # delay between each sampling
time_to_create_dump = config.getint('main', 'time_per_file', fallback=3600)  # time in seconds between each report
email = config.get('main', 'email', fallback='admin@hermes.radio') # destination email of the data

path="/var/spool/sensors/"

agps_thread = AGPS3mechanism()  # Instantiate AGPS3 Mechanisms
agps_thread.stream_data()  # From localhost (), or other hosts, by example, (host='gps.ddns.net')
agps_thread.run_thread()  # Throttle time to sleep after an empty lookup, default 0.2 second, default daemon=True

controller = EpeverChargeController("/dev/ttyUSB1", 1)

try:
    os.mkdir(path)
except OSError as error:
    print("Directory " + path + " already created. Good.")

time.sleep(1)
next_time = time.time() + delay
counter = 0

ct = datetime.datetime.now().isoformat(timespec='minutes')
path_file = os.path.join(path, ct + ".csv")
fd = open(path_file, "w", 1)
fd.write("Time Stamp, Latitude, Longitude, Vbatt, Abatt, Vload, Aload, Vsolar, Asolar, Temp, SOC\n")


while True:

    if counter >= time_to_create_dump:
        fd.close()
        cmd_string = 'enc_sensors ' + path_file + ' ' + email + ' &'
        print(cmd_string)
        os.system(cmd_string);

        ct = datetime.datetime.now().isoformat(timespec='minutes')
        path_file = os.path.join(path, ct + ".csv")
        fd = open(path_file,"w", 1)
        fd.write("Time Stamp, Latitude, Longitude, Vbatt, Abatt, Vload, Aload, Vsolar, Asolar, Temp, SOC\n")
        counter = 0


    time.sleep(max(0, next_time - time.time()))

    fd.write(datetime.datetime.now().strftime("%s") + ",")
    lat = str(agps_thread.data_stream.lat)
    if lat == "n/a":
        fd.write("0,")
    else:
        fd.write(lat + ",")
    lon = str(agps_thread.data_stream.lon)
    if lon == "n/a":
        fd.write("0,")
    else:
        fd.write(lon + ",")
    fd.write(str(controller.get_battery_voltage()) + ',')
    fd.write(str(controller.get_battery_current()) + ',')
    fd.write(str(controller.get_load_voltage()) + ',')
    fd.write(str(controller.get_load_current()) + ',')
    fd.write(str(controller.get_solar_voltage()) + ',')
    fd.write(str(controller.get_solar_current()) + ',')
    fd.write(str(controller.get_controller_temperature()) + ',')
    fd.write(str(controller.get_battery_state_of_charge()) + "\n")

    next_time += delay
    counter += delay
