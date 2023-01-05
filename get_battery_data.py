#!/usr/bin/python3
from time import sleep

from epevermodbus.driver import EpeverChargeController
from gps3.agps3threaded import AGPS3mechanism


controller = EpeverChargeController("/dev/ttyUSB1", 1)

agps_thread = AGPS3mechanism()  # Instantiate AGPS3 Mechanisms
agps_thread.stream_data()  # From localhost (), or other hosts, by example, (host='gps.ddns.net')
agps_thread.run_thread()  # Throttle time to sleep after an empty lookup, default 0.2 second, default daemon=True

while True:
    print('---------------------')
    print(                   agps_thread.data_stream.time)
    print('Lat:{}   '.format(agps_thread.data_stream.lat))
    print('Lon:{}   '.format(agps_thread.data_stream.lon))
    print('Speed:{} '.format(agps_thread.data_stream.speed))
    print('Course:{}'.format(agps_thread.data_stream.track))
    print('---------------------')
    vbatt = controller.get_battery_voltage()
    cbatt = controller.get_battery_current()
    soc = controller.get_battery_state_of_charge()
    print(str(vbatt) + ',' + str(cbatt) + ',' + str(soc), end ='')
    sleep(1)

# controller.get_solar_voltage
# controller.get_load_voltage
#
# controller.get_solar_current
# controller.get_load_current
