#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    FILE *fin = fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "w");

    uint32_t time_stamp;
    float lat, lon;

    uint8_t buffer[12];

    // skip the first line
    fprintf(fout, "Time Stamp, Latitude, Longitude\n");
    while (fread(buffer, 12, 1, fin))
    {
        fprintf(fout, "%u,%f,%f\n", (uint32_t) buffer[0], (float) buffer[4], (float) buffer[8]);
    }
}
