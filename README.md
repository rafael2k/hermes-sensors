# hermes-sensors

A suite of solutions for grabing information from sensors (GPS, change controller, ...) and send them through HERMES.

The implementation consists of a daemon which captures the data from the sensors (sensors_daemon\*.py) and 
two commands which compress and decompress the captured data (enc_sensors and dec_sensors). The transport
of the file is carried over UUCP through the use of uux. The station which captures the sensor data
can be "gps-only" or "gps+battery" types. The server station receives and routes the data over email.



## How to use

### Dependencies:

The Python libraries used are:

 * GPS3: https://github.com/Rhizomatica/gps3
 * epevermodbus: https://github.com/Rhizomatica/epevermodbus

### Compilation

The setup for capture sensors data station can be "gps+battery" or "gps-only".

To compile for "gps-only" use:

$ CFLAGS="-DOPERATION_MODE=0" make

To compile for "gps+battery" use:

$ CFLAGS="-DOPERATION_MODE=1" make

To compile for server station:

$ make

### installation

For "gps-only" stations use:

$ make install_gps_only

For "battery+gps" stations use:

$ make install_gps_only

For server station:

$ make install


# Author

Rafael Diniz at Rhizomatica

