/* hermes-sensors
 *
 * Copyright (C) 2023-2024 Rhizomatica
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#define GPS_ONLY 0
#define GPS_AND_BATTERY 1

#ifndef OPERATION_MODE
#define OPERATION_MODE GPS_ONLY
#endif

#define MAX_FILENAME 4096
#define CMD_LENGTH 4096


int main(int argc, char *argv[])
{
    bool emergency_flag = false;
    char *input_file = NULL;
    char *email = NULL;
    char *from = NULL;

    if (argc < 2)
    {
    manual:
        fprintf(stderr, "Usage: \n%s [-i input.csv] [-e to_email] [-f from_email] [-s]\n", argv[0]);
        fprintf(stderr, "%s -h\n", argv[0]);
        fprintf(stderr, "\nOptions:\n");
        fprintf(stderr, " -i input.csv       Input CSV path\n");
        fprintf(stderr, " -e to_email        Destination email (To)\n");
        fprintf(stderr, " -f from_email      Source email (From)\n");
        fprintf(stderr, " -s                 Enable emergency flag (SOS)\n");
        fprintf(stderr, " -h                 Prints this help.\n\n");

        exit(EXIT_FAILURE);
    }

    int opt;
    while ((opt = getopt(argc, argv, "i:e:f:sh")) != -1)
    {
        switch (opt)
        {
        case 'h':
            goto manual;
            break;
        case 's':
            emergency_flag = true;
            break;
        case 'i':
            input_file = optarg;
            break;
        case 'e':
            email = optarg;
            break;
        case 'f':
            from = optarg;
            break;
        default:
            goto manual;
        }
    }

    // csv input
    FILE *fin = fopen(input_file, "r");

    // write the payload to .bin file
    char raw_filename[MAX_FILENAME];
    sprintf(raw_filename, "/tmp/enc_sensors.bin.%d", getpid ());
    FILE *fout = fopen(raw_filename, "w");

    uint32_t time_stamp;
    uint8_t soc;
    float lat, lon, vbatt, abatt, vload, aload, vsolar, asolar, temp;
    // skip the first line
    while (getc(fin) != '\n');

#if OPERATION_MODE == GPS_AND_BATTERY
    while (fscanf(fin, "%u,%f,%f,%f,%f,%hhu\n", &time_stamp, &lat, &lon, &vbatt, &abatt, &vload, &aload, &vsolar, &asolar, &temp, &soc) > 0)
#endif
#if OPERATION_MODE == GPS_ONLY
    while (fscanf(fin, "%u,%f,%f\n", &time_stamp, &lat, &lon) > 0)
#endif
    {
        // printf("%u,%f,%f,%f,%f,%hhu\n", time_stamp, lat, lon, vbatt, abatt, soc);
        fwrite(&time_stamp, 4, 1, fout);
        fwrite(&lat, 4, 1, fout);
        fwrite(&lon, 4, 1, fout);
#if OPERATION_MODE == GPS_AND_BATTERY
        fwrite(&vbatt, 4, 1, fout);
        fwrite(&abatt, 4, 1, fout);
        fwrite(&vload, 4, 1, fout);
        fwrite(&aload, 4, 1, fout);
        fwrite(&vsolar, 4, 1, fout);
        fwrite(&asolar, 4, 1, fout);
        fwrite(&temp, 4, 1, fout);
        fwrite(&soc, 1, 1, fout);
#endif
    }
    fclose(fin);
    fclose(fout);
    // TODO: add an option to delete csv
    // unlink(input_file);

    // compress the file
    char compressed_payload_filename[MAX_FILENAME];
    sprintf(compressed_payload_filename, "/tmp/enc_sensors.paq.%d", getpid ());
    char cmd_compress[CMD_LENGTH];
    sprintf(cmd_compress, "paq8px -3 %s %s", raw_filename, compressed_payload_filename);
    system(cmd_compress);
    unlink(raw_filename);

    // sent over uux
    char cmd_string[MAX_FILENAME];
    sprintf(cmd_string, "uux -r - gw\\!dec_sensors %s -e %s -f %s %s < %s",
            OPERATION_MODE == GPS_AND_BATTERY ? "-b" : "-g",
            email, from, emergency_flag ? "-s" : "",
            compressed_payload_filename);

    printf("%s\n", cmd_string);
    system(cmd_string);

    unlink(compressed_payload_filename);

    if (emergency_flag)
        system("sudo uucico -D -S gw");

    return 0;
}
