/* uuxsensor
 * Copyright (C) 2023 Rhizomatica
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

#define GPS_ONLY 0
#define GPS_AND_BATTERY 1

#define EMAIL "rafael@riseup.net"

// #define OPERATION_MODE GPS_AND_BATTERY
#define OPERATION_MODE GPS_ONLY

#define BUF_SIZE 4096
#define MAX_FILENAME 4096
#define CMD_LENGTH 4096


int main(int argc, char *argv[])
{
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
    sprintf(compressed_payload_filename, "/tmp/uuxsensor.comp.%d", getpid ());
    FILE *compressed_fd = fopen(compressed_payload_filename, "w");
    fwrite(message_payload, message_size, 1, compressed_fd);
    fclose(compressed_fd);
    free(message_payload);


    // uncompress the file
    char uncompressed_payload_filename[MAX_FILENAME];
    sprintf(uncompressed_payload_filename, "/tmp/uuxsensor.decomp.%d", getpid ());
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


    // now write the csv
    char csv_output_filename[MAX_FILENAME];
    sprintf(csv_output_filename, "/tmp/uuxsensor.csv.%d", getpid ());
    FILE *csv_fd = fopen(csv_output_filename, "w");

#if (OPERARION_MODE == GPS_AND_BATTERY)
    fprintf(csv_fd, "Time Stamp, Latitude, Longitude, Vbatt, Abatt, SOC\n");
#endif
#if (OPERARION_MODE == GPS_ONLY)
    fprintf(csv_fd, "Time Stamp, Latitude, Longitude\n");
#endif


    uint8_t *buffer = uncompressed_payload_buffer;
    while (buffer < uncompressed_payload_buffer + uncompressed_payload_size)
    {
        uint32_t time_stamp;
        uint8_t soc;
        float lat, lon, vbatt, abatt;
        struct tm  ts;
        char       buf[80];

        time_stamp = *((uint32_t *)buffer);
        ts = *localtime((time_t *)&time_stamp);
// strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
        lat = *((float *) (buffer + 4));
        lon = *((float *) (buffer + 8));
#if OPERATION_MODE == GPS_AND_BATTERY
        vbatt = *((float *) (buffer + 12));
        abatt = *((float *) (buffer + 16));
        soc = *((uint8_t *) (buffer + 20));

        fprintf(csv_fd, "%s,%f,%f,%f,%f,%hhu\n", buf, lat, lon, vbatt, abatt, soc);
        buffer = buffer + 21;
#endif
#if OPERATION_MODE == GPS_ONLY
        fprintf(csv_fd, "%s,%f,%f\n", buf, lat, lon);
        buffer = buffer + 12;
#endif
    }
    fclose(csv_fd);

    free(uncompressed_payload_buffer);

    // prepare to send email...
    char *email = NULL;
    if (argc > 1)
        email = argv[1];

    char mail_cmd[CMD_LENGTH];

    if (email != NULL)
    {
        sprintf(mail_cmd, "echo -e \"HERMES monitoring system email\" | mail --content-type=text/csv --encoding=base64 --attach=\"%s\" -s \"HERMES SYSTEM\" \"%s\"", csv_output_filename, email);
        printf("%s\n", mail_cmd);
        system(mail_cmd);
        unlink(csv_output_filename);
    }
    else {
        sprintf(mail_cmd, "echo -e \"HERMES monitoring system email\" | mail --content-type=text/csv --encoding=base64 --attach=\"%s\" -s \"HERMES SYSTEM\" %s", csv_output_filename, EMAIL);
        printf("%s\n", mail_cmd);
        system(mail_cmd);
//        unlink(csv_output_filename);
        printf("Output is at: %s\n", csv_output_filename);
    }

    unlink(compressed_payload_filename);
    unlink(uncompressed_payload_filename);


}
