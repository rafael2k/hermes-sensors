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
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>


#define GPS_ONLY 0
#define GPS_AND_BATTERY 1

#ifndef OPERATION_MODE
#define OPERATION_MODE GPS_ONLY
#endif


#define BUF_SIZE 4096
#define MAX_FILENAME 4096
#define CMD_LENGTH 4096


int main(int argc, char *argv[])
{
    int operation_mode;
    char email[BUF_SIZE];
    char from[BUF_SIZE];
    bool emergency_flag = false;
    bool from_set = false;

    if (argc < 2)
    {
    manual:
        fprintf(stderr, "Usage: \n%s [-g -b] [-e e-mail] [-f e-mail] [-s]\n", argv[0]);
        fprintf(stderr, "%s -h\n", argv[0]);
        fprintf(stderr, "\nOptions:\n");
        fprintf(stderr, " -g                 Runs GPS_ONLY mode\n");
        fprintf(stderr, " -b                 Runs GPS_AND_BATTERY mode\n");
        fprintf(stderr, " -s                 Enable emergency flag (SOS)\n");
        fprintf(stderr, " -e e-mail          Sets the destination e-mail address\n");
        fprintf(stderr, " -f e-mail          Sets the source e-mail address\n");
        fprintf(stderr, " -h                 Prints this help.\n\n");
        exit(EXIT_FAILURE);
    }

    int opt;
    while ((opt = getopt(argc, argv, "hgbse:f:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            goto manual;
            break;
        case 'g':
            operation_mode = GPS_ONLY;
            break;
        case 'b':
            operation_mode = GPS_AND_BATTERY;
            break;
        case 's':
            emergency_flag = true;
            break;
        case 'e':
            strcpy(email, optarg);
            break;
        case 'f':
            strcpy(from, optarg);
            from_set = true;
            break;
        default:
            goto manual;
        }
    }


    // read the compressed payload of the sensor
    size_t buffer_size;
    size_t message_size;
    uint8_t *message_payload; // dynamic size, read from stdin
    char tmp_buffer[BUF_SIZE];
    message_size = fread(tmp_buffer, 1, BUF_SIZE, stdin);
    message_payload = malloc(message_size);
    memcpy(message_payload, tmp_buffer, message_size);
    while ( !feof(stdin) )
    {
        size_t needle = message_size;
        buffer_size = fread(tmp_buffer, 1, BUF_SIZE, stdin);
        message_size += buffer_size;
        message_payload = realloc(message_payload, message_size);
        memcpy(message_payload + needle, tmp_buffer, buffer_size);
    }

    // write the payload to file
    char compressed_payload_filename[MAX_FILENAME];
    sprintf(compressed_payload_filename, "/tmp/dec_sensors.comp.%d", getpid ());
    FILE *compressed_fd = fopen(compressed_payload_filename, "w");
    fwrite(message_payload, message_size, 1, compressed_fd);
    fclose(compressed_fd);
    free(message_payload);


    // uncompress the file
    char uncompressed_payload_filename[MAX_FILENAME];
    sprintf(uncompressed_payload_filename, "/tmp/dec_sensors.decomp.%d", getpid ());
    char cmd_uncompress[CMD_LENGTH];
    sprintf(cmd_uncompress, "paq8px -d %s %s", compressed_payload_filename, uncompressed_payload_filename);
    printf("%s\n", cmd_uncompress);
    system(cmd_uncompress);



    // read the uncompressed data to memory
    FILE *uncompressed_payload = fopen(uncompressed_payload_filename, "r");
    fseek(uncompressed_payload, 0, SEEK_END);
    long uncompressed_payload_size = ftell(uncompressed_payload);
    fseek(uncompressed_payload, 0, SEEK_SET);  /* same as rewind(f); */
    uint8_t *uncompressed_payload_buffer =  malloc(uncompressed_payload_size);
    fread(uncompressed_payload_buffer, uncompressed_payload_size, 1, uncompressed_payload);
    fclose(uncompressed_payload);

    // get first timestamp to put in the file name
    char first_timestamp[80];
    struct tm  ts; uint32_t time_stamp;

    time_stamp = *((uint32_t *)uncompressed_payload_buffer);
    ts = *localtime((time_t *)&time_stamp);
    strftime(first_timestamp, sizeof(first_timestamp), "%Y-%m-%d_%H.%M.%S", &ts);

    // get hostname from email
    char *station_name = strstr(from, "@");
    station_name++;

    // now write the csv
    char csv_output_filename[MAX_FILENAME];
    sprintf(csv_output_filename, "/tmp/%s%s-%s.csv", emergency_flag?"SOS-":"", station_name, first_timestamp);
    FILE *csv_fd = fopen(csv_output_filename, "w");

    if (operation_mode == GPS_AND_BATTERY)
        fprintf(csv_fd, "Time Stamp, Latitude, Longitude, Vbatt, Abatt, Vload, Aload, Vsolar, Asolar, Temp, SOC\n");
    if (operation_mode == GPS_ONLY)
        fprintf(csv_fd, "Time Stamp, Latitude, Longitude\n");

    uint8_t *buffer = uncompressed_payload_buffer;

    while (buffer < uncompressed_payload_buffer + uncompressed_payload_size)
    {
        uint8_t soc;
        float lat, lon, vbatt, abatt, vload, aload, vsolar, asolar, temp;
        struct tm  ts; uint32_t time_stamp;

        char       buf[80];
        time_stamp = *((uint32_t *)buffer);
        ts = *localtime((time_t *)&time_stamp);
// strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
        lat = *((float *) (buffer + 4));
        lon = *((float *) (buffer + 8));
        if (operation_mode == GPS_AND_BATTERY)
        {
            vbatt = *((float *) (buffer + 12));
            abatt = *((float *) (buffer + 16));
            vload = *((float *) (buffer + 20));
            aload = *((float *) (buffer + 24));
            vsolar = *((float *) (buffer + 28));
            asolar = *((float *) (buffer + 32));
            temp = *((float *) (buffer + 36));
            soc = *((uint8_t *) (buffer + 40));

            fprintf(csv_fd, "%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%hhu\n", buf, lat, lon, vbatt, abatt, vload, aload, vsolar, asolar, temp, soc);
            buffer = buffer + 41;
        }
        if (operation_mode == GPS_ONLY)
        {
            fprintf(csv_fd, "%s,%f,%f\n", buf, lat, lon);
            buffer = buffer + 12;
        }
    }
    fclose(csv_fd);

    free(uncompressed_payload_buffer);


    char mail_cmd[CMD_LENGTH];

    // TODO: write station in the subject, and add a SOS in case of emergency
    sprintf(mail_cmd, "echo %sHERMES GPS DATA | mail %s%s --content-type=text/csv --encoding=base64 --attach=\"%s\" -s \"%s HERMES SYSTEM\" \"%s\"", emergency_flag?"SOS ": "",
            from_set?"-r " : "", from_set?from:"",
            csv_output_filename, emergency_flag?"SOS ": "", email);
    printf("%s\n", mail_cmd); // TODO: Remove debug
    system(mail_cmd);
    unlink(csv_output_filename);

    unlink(compressed_payload_filename);
    unlink(uncompressed_payload_filename);

    return 0;
}
