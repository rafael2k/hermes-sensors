#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    FILE *fin = fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "w");

    uint32_t time_stamp;
    float lat, lon;

    // skip the first line
    while (getc(fin) != '\n');

    while (fscanf(fin, ("%u,%f,%f\n"), &time_stamp, lat, lon) != EOF)
    {
        fwrite(&time_stamp, 4, 1, fout);
        fwrite(&lat, 4, 1, fout);
        fwrite(&lon, 4, 1, fout);
    }
}
