#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    FILE *fin = fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "w");

    uint32_t time_stamp;
    uint8_t soc;
    float lat, lon, vbatt, abatt;

    uint8_t buffer[21];

    fprintf(fout, "Time Stamp, Latitude, Longitude, Vbatt, Abatt, SOC\n");

    while (fread(buffer, 21, 1, fin))
    {
        time_stamp = *((uint32_t *)buffer);
        lat = *((float *) (buffer + 4));
        lon = *((float *) (buffer + 8));
        vbatt = *((float *) (buffer + 12));
        abatt = *((float *) (buffer + 16));
        soc = *((uint8_t *) (buffer + 20));
        fprintf(fout, "%u,%f,%f,%f,%f,%hhu\n", time_stamp, lat, lon, vbatt, abatt, soc);
    }
}
