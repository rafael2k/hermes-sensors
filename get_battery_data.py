#!/usr/bin/python3

from epevermodbus.driver import EpeverChargeController


controller = EpeverChargeController("/dev/ttyUSB1", 1)

vbatt = controller.get_solar_voltage()
cbatt = controller.get_battery_current
soc = controller.get_battery_state_of_charge
print(vbatt + ',' + cbatt + ',' + soc)

# controller.get_load_voltage
# controller.get_battery_voltage

# controller.get_load_current
# controller.get_solar_current
