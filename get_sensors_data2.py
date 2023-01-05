#!/usr/bin/python3

from time import sleep
from threading import Event, Thread
from epevermodbus.driver import EpeverChargeController
from gps3.agps3threaded import AGPS3mechanism


controller = EpeverChargeController("/dev/ttyUSB1", 1)

agps_thread = AGPS3mechanism()  # Instantiate AGPS3 Mechanisms
agps_thread.stream_data()  # From localhost (), or other hosts, by example, (host='gps.ddns.net')
agps_thread.run_thread()  # Throttle time to sleep after an empty lookup, default 0.2 second, default daemon=True

sleep(1)

print('timestamp, latitude, longitude, Vbatt, Abatt, SOC')

stopped = Event()

def sensors():
    print(agps_thread.data_stream.time + ',', end='')
    print(str(agps_thread.data_stream.lat) + ',', end='')
    print(str(agps_thread.data_stream.lon) + ',', end='')
    print(str(controller.get_battery_voltage()) + ',', end='')
    print(str(controller.get_battery_current()) + ',', end='')
    print(str(controller.get_battery_state_of_charge()))

#           print(agps_thread.data_stream.speed
#            print(agps_thread.data_stream.track)

from threading import Timer

class RepeatedTimer(object):
    def __init__(self, interval, function, *args, **kwargs):
        self._timer     = None
        self.interval   = interval
        self.function   = function
        self.args       = args
        self.kwargs     = kwargs
        self.is_running = False
        self.start()

    def _run(self):
        self.is_running = False
        self.start()
        self.function(*self.args, **self.kwargs)

    def start(self):
        if not self.is_running:
            self._timer = Timer(self.interval, self._run)
            self._timer.start()
            self.is_running = True

    def stop(self):
        self._timer.cancel()
        self.is_running = False


# start timer
timer = RepeatedTimer(1, sensors)

# stop timer... if we want to.
#timer.stop()
