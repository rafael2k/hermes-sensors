##
# HERMES sensors
#
# @file
# @version 0.1

all: encode decode

encode: encode.c
	gcc encode.c -o encode

decode: decode.c
	gcc decode.c -o decode

.PHONY:clean
clean:
	rm -f decode encode

# end
