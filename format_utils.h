#ifndef RCB_FILE_SYSTEM_FORMAT_UTILS_H
#define RCB_FILE_SYSTEM_FORMAT_UTILS_H

#include <stdio.h>
#include <errno.h>
#include "messages.h"
#include "struct_utils.h"
#include "generic_utils.h"

unsigned int write_boot_record(FILE *device, unsigned int sect_size, long device_size) {
    boot_record btr;
    printf("%li", device_size);
    fseek(device, 0, SEEK_SET);
    strcpy(btr.rcb, SIGNATURE);
    btr.bytes_per_sector = (unsigned short) sect_size;
    btr.sectors_per_rcb = (unsigned short) (RCB_TABLE_SIZE / btr.bytes_per_sector);
    btr.reserved_sectors = (unsigned short) (1 + btr.sectors_per_rcb);
    btr.entry_directory = (unsigned short) (btr.bytes_per_sector / RCB_DATA_TABLE);
    btr.bytes_per_partition = (unsigned int) device_size;
    btr.sectors_per_disk = (unsigned short) (btr.bytes_per_partition / btr.bytes_per_sector);

    fwrite(&btr, 1, sizeof(struct boot_record), device);
    return btr.sectors_per_rcb;
}

void write_root_dir(FILE *device, unsigned int sect_size, unsigned int sectors_per_rcb){
    root_dir dir;
    fseek(device, sect_size * (1 + sectors_per_rcb), SEEK_SET);
}

int hard_format(const char *device_name, unsigned int sect_size) {
    FILE *device;
    long device_size;

    device = fopen(device_name, "rb+");
    if(device == NULL){
        print_invalid_device(strerror(errno));
        return 1;
    }
    device_size = get_device_size(device);

    while (fgetc (device) != EOF) {
        fseek(device, -1, SEEK_CUR);
        fputc(0, device);
    }

    unsigned int sectors_per_rcb = write_boot_record(device, sect_size, device_size);
    write_root_dir(device, sect_size, sectors_per_rcb);

    fclose(device);
    return 0;
}

int soft_format(const char *device_name, int size) {
    //
    return 0;
}

int format_device(const char *device_name) {
    char option;
    unsigned int sect_size;
    bool hard = false;
    printf("Are you sure you want to format the device? [Y/n] ");
    scanf("%c", &option);
    getchar();
    if (option != 'y' && option != 'Y') {
        return 0;
    }
    printf("Do you want to hard format your device (may be slower)? [Y/n] ");
    scanf("%c", &option);
    getchar();
    if (option == 'y' || option == 'Y') {
        hard = true;
    }
    printf("Size of sectors in bytes (must be a power of two): ");
    scanf("%d", &sect_size);
    sect_size = parse_sect(sect_size);
    getchar();

    return hard ? hard_format(device_name, sect_size) : soft_format(device_name, sect_size);
}

#endif