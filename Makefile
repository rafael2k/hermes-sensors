##
# HERMES sensors
#
# @file
# @version 0.1

CC=gcc

all: encode-gps_only decode-gps_only encode-gps_battery decode-gps_battery dec_sensors enc_sensors

encode-gps_only: encode-gps_only.c
	$(CC) encode-gps_only.c -o encode-gps_only

decode-gps_only: decode-gps_only.c
	$(CC) decode-gps_only.c -o decode-gps_only

encode-gps_battery: encode-gps_battery.c
	$(CC) encode-gps_battery.c -o encode-gps_battery

decode-gps_battery: decode-gps_battery.c
	$(CC) decode-gps_battery.c -o decode-gps_battery

dec_sensors: dec_sensors.c
	$(CC) dec_sensors.c -o dec_sensors

enc_sensors: enc_sensors.c
	$(CC) enc_sensors.c -o enc_sensors

install: encode-gps_only decode-gps_only encode-gps_battery decode-gps_battery sensors_daemon.py sensors.service
	install -D encode-gps_only /usr/bin/encode-gps_only
	install -D decode-gps_only /usr/bin/decode-gps_only
	install -D encode-gps_battery /usr/bin/encode-gps_battery
	install -D decode-gps_battery /usr/bin/decode-gps_battery
	install -D dec_sensors /usr/bin/dec_sensors
	install -D enc_sensors /usr/bin/enc_sensors
	install -D sensors_daemon.py /usr/bin/sensors_daemon.py
	install -m 644 -D sensors.service $(DESTDIR)/etc/systemd/system/sensors.service

.PHONY:clean
clean:
	rm -f decode-gps_only encode-gps_only encode-gps_battery decode-gps_battery enc_sensors dec_sensors

# end
