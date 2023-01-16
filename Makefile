##
# HERMES sensors
#
# @file
# @version 0.1

all: encode decode

encode:
	gcc encode.c -o encode

decode:
	gcc decode.c -o decode

.PHONY:clean
clean:
	rm -f decode encode

# end
