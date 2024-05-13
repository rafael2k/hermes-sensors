##
# HERMES sensors
#
# @file
# @version 0.1

CC=gcc
CFLAGS ?= -DOPERATION_MODE=0

all: dec_sensors enc_sensors

encode-gps_only: encode-gps_only.c
	$(CC) encode-gps_only.c -o encode-gps_only

decode-gps_only: decode-gps_only.c
	$(CC) decode-gps_only.c -o decode-gps_only

encode-gps_battery: encode-gps_battery.c
	$(CC) encode-gps_battery.c -o encode-gps_battery

decode-gps_battery: decode-gps_battery.c
	$(CC) decode-gps_battery.c -o decode-gps_battery

dec_sensors: dec_sensors.c
	$(CC) $(CFLAGS) dec_sensors.c -o dec_sensors

enc_sensors: enc_sensors.c
	$(CC) $(CFLAGS) enc_sensors.c -o enc_sensors

install_gps_only: sensors_daemon-gps_only.py sensors.service enc_sensors dec_sensors
	install -D dec_sensors $(DESTDIR)/usr/bin/dec_sensors
	install -D enc_sensors $(DESTDIR)/usr/bin/enc_sensors
	install -D sensors_daemon-gps_only.py $(DESTDIR)/usr/bin/sensors_daemon.py
	install -m 644 -D sensors.service $(DESTDIR)/etc/systemd/system/sensors.service
	install -m 644 -D config/sensors.ini $(DESTDIR)/etc/sbitx/sensors.ini

install_gps_battery: sensors_daemon-gps_battery.py sensors.service enc_sensors dec_sensors
	install -D dec_sensors $(DESTDIR)/usr/bin/dec_sensors
	install -D enc_sensors $(DESTDIR)/usr/bin/enc_sensors
	install -D sensors_daemon-gps_battery.py $(DESTDIR)/usr/bin/sensors_daemon.py
	install -m 644 -D sensors.service $(DESTDIR)/etc/systemd/system/sensors.service
	install -m 644 -D config/sensors.ini $(DESTDIR)/etc/sbitx/sensors.ini

install:
	install -D dec_sensors /usr/bin/dec_sensors
	install -D enc_sensors /usr/bin/enc_sensors

.PHONY:clean
clean:
	rm -f decode-gps_only encode-gps_only encode-gps_battery decode-gps_battery enc_sensors dec_sensors

# end
