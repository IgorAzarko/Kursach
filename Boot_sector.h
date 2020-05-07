#ifndef BOOT_SECTOR_H
#define BOOT_SECTOR_H

#include <string.h>
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>




struct Boot_Sector {
    unsigned char bootstrap[3];                                  // 0-2
    unsigned char OEM_name[8];                                   // 3-10
    unsigned char bytes_per_sector[2];                           // 11-12
    unsigned char sectors_per_cluster;                           // 13
    unsigned char number_reserved_sectors[2];                    // 14-15
    unsigned char number_FAT_copies;                             // 16
    unsigned char number_root_entries[2];                        // 17-18
    unsigned char total_number_sectors_NOFAT32[2];               // 19-20
    unsigned char media_descriptor_type;                         // 21
    unsigned char number_of_sectors_per_FAT_NOFAT32[2];          // 22-23; 
    unsigned char number_of_sectors_per_track[2];                // 24-25
    unsigned char number_of_heads[2];                            // 26-27
    unsigned char number_of_hidden_sectors[4];                   // 28-31
    unsigned char total_number_of_sectors[4];                    // 32-35
    unsigned char number_of_sectors_per_FAT[4];                  // 36-39
    unsigned char mirror_flags[2];                               // 40-41
    unsigned char filesystem_version[2];                         // 42-43
    unsigned char first_cluster_root_directory[4];               // 44-47
    unsigned char filesystem_information_sector[2];              // 48-49
    unsigned char backup_boot_sector_location[2];                // 50-51
    unsigned char reserved[12];                                  // 52-63
    unsigned char locival_drive_number;                          // 64
    unsigned char reserver_for_WindowsNT;                        // 65
    unsigned char extended_signature_0x29;                       // 66
    unsigned char serial_number_partition[4];                    // 67-70
    unsigned char volume_label[11];                              // 71-81
    unsigned char filesystem_type[8];                            // 82-89
    unsigned char bootstrap_data[420];                           // 90-509
    unsigned char signature_55AA[2];                              // 510-511
};



unsigned char* reversed_short_to_char(unsigned short int num) {
    unsigned char str[2];
    unsigned char first = (unsigned char)(num);
    unsigned char second = (unsigned char)(num >> 8);


    str[0] = first;
    str[1] = second;
    return str;
}


unsigned char* reversed_int_to_char(unsigned int num) {
    unsigned char str[4];
    unsigned char first = (unsigned char)(num);
    unsigned char second = (unsigned char)(num >> 8);
    unsigned char third = (unsigned char)(num >> 16);
    unsigned char fourth = (unsigned char)(num >> 24);

    str[0] = first;
    str[1] = second;
    str[2] = third;
    str[3] = fourth;

    return str;
}



bool set_default_Boot_sector(HANDLE descriptor, int quantity_all_sectors, int size_table_FAT_sector)
{
    struct Boot_Sector default_Boot_Sector {

        { 0xEB, 0x3C, 0x90 },                                               // 0-2
        { 'I', 'G', 'O', 'R', 'G','E', 'L', 'Y' },                          // 3-10
        { 0x00, 0x02 },                                                     // 11-12
        { 1 },                                                              // 13
        { 0x20, 0x00 },                                                     // 14-15
        { 2 },                                                              // 16
        { 0x00 ,0x00 },                                                     // 17-18
        { 0x00 ,0x00 },                                                     // 19-20
        { 0xf8, },                                                          // 21
        { 0x00 ,0x00 },                                                     // 22-23; 
        { 0x0C, 0x00 },                                                     // 24-25
        { 0x02, 0x00 },                                                     // 26-27
        { 0x00 ,0x00,0x00 ,0x00 },                                          // 28-31
        {    /*посчитать*/ },                                               // 32-35
        {     /*посчитать*/ },                                              // 36-39
        { 0x00 ,0x00 },                                                     // 40-41
        { 0x00 ,0x00 },                                                     // 42-43
        { 0x02, 0x00 },                                                     // 44-47
        { 0x01, 0x00 },                                                     // 48-49
        { 0x06, 0x00 },                                                     // 50-51
        {    /*резерв*/ },                                                  // 52-63
        { 0x00 },                                                           // 64
        { 0x00 },                                                           // 65
        { 0x29 },                                                           // 66
        { 0x00 ,0x00 ,0x00 ,0x00 },                                         // 67-70
        { 'I', 'G', 'O', 'R', ' ','A', 'Z', 'A','R', 'K', 'O' },            // 71-81
        { 'F', 'A', 'T', '3', '2',' ', ' ', ' ' },                          // 82-89
        {    /*резерв*/ },                                                  // 90-509
        { 0x55, 0xAA },                                                     // 510-511

    };

    unsigned char* temp = reversed_int_to_char(quantity_all_sectors);
    memcpy((void*)default_Boot_Sector.total_number_of_sectors, (void*)temp, 4);
    temp = reversed_int_to_char(size_table_FAT_sector);
    memcpy((void*)default_Boot_Sector.number_of_sectors_per_FAT, (void*)temp, 4);

    for (int i = 0; i < 12; i++) {
        default_Boot_Sector.reserved[i] = 0;
    }
    for (int i = 0; i < 410; i++) {
        default_Boot_Sector.bootstrap_data[i] = 0;
    }


    // дальше надо записать эту структуру в файл

    if (WriteFile(descriptor, &default_Boot_Sector, sizeof(struct Boot_Sector), NULL, NULL) == 0) {
        printf("\n Can not write Boot Sector in Your device. \n");
        return false;
    }
    return true;
}



const char* device_path = "F:/";

void error_open_device(const char* file_path)                   // нужны
{
    printf("\n \'%s\' : No such directory, or cannot open your device for reading. Please check Your device and try again\n", file_path);
    exit(0);
}

HANDLE open_device(const char* device_path)                     // нужны
{
    HANDLE hDevice =
        CreateFileW(L"\\\\.\\f:", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);


    if (hDevice == INVALID_HANDLE_VALUE) // невозможно открыть устройство
    {
        error_open_device(device_path);

    }
    return hDevice;
}



bool seek_descriptor(HANDLE device_descriptor, unsigned long long offset)
{
    _LARGE_INTEGER integer;
    integer.QuadPart = offset;
    if (SetFilePointerEx(device_descriptor, integer, NULL, FILE_BEGIN) == 0) {
        printf("\n Move's error in file \n");
        return false;
    }

    return true;
}


#endif BOOT_SECTOR_H

