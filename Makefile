##
# HERMES sensors
#
# @file
# @version 0.1

CC=gcc

all: encode-gps_only decode-gps_only encode-gps_battery decode-gps_battery uuxsensor

encode-gps_only: encode-gps_only.c
	$(CC) encode-gps_only.c -o encode-gps_only

decode-gps_only: decode-gps_only.c
	$(CC) decode-gps_only.c -o decode-gps_only

encode-gps_battery: encode-gps_battery.c
	$(CC) encode-gps_battery.c -o encode-gps_battery

decode-gps_battery: decode-gps_battery.c
	$(CC) decode-gps_battery.c -o decode-gps_battery

uuxsensor: uuxsensor.c
	$(CC) uuxsensor.c -o uuxsensor

install: encode-gps_only decode-gps_only encode-gps_battery decode-gps_battery
	install -D encode-gps_only /usr/bin/encode-gps_only
	install -D decode-gps_only /usr/bin/decode-gps_only
	install -D encode-gps_battery /usr/bin/encode-gps_battery
	install -D decode-gps_battery /usr/bin/decode-gps_battery
	install -D uuxsensor /usr/bin/uuxsensor

.PHONY:clean
clean:
	rm -f decode-gps_only encode-gps_only encode-gps_battery decode-gps_battery

# end
