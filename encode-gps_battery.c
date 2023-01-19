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


    // skip the first line
    while (getc(fin) != '\n');

    while (fscanf(fin, "%u,%f,%f,%f,%f,%hhu\n", &time_stamp, &lat, &lon, &vbatt, &abatt, &soc) != EOF)
    {
        printf("%u,%f,%f,%f,%f,%hhu\n", time_stamp, lat, lon, vbatt, abatt, soc);
        fwrite(&time_stamp, 4, 1, fout);
        fwrite(&lat, 4, 1, fout);
        fwrite(&lon, 4, 1, fout);
        fwrite(&vbatt, 4, 1, fout);
        fwrite(&abatt, 4, 1, fout);
        fwrite(&soc, 1, 1, fout);
    }
}
