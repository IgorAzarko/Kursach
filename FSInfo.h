#include "Boot_sector.h"

struct FSInfo {
    unsigned char first_signature[4];
    unsigned char first_reserved[480];
    unsigned char second_signature[4];
    unsigned char free_cluster_count[4];
    unsigned char next_free_cluster[4];
    unsigned char second_reserved[12];
    unsigned char third_signature[4];
};


bool set_default_FSInfo(HANDLE descriptor, int quantity_empty_sectors, int begin_date)
{
    struct FSInfo default_FSInfo {
        { 0x52, 0x52, 0x61, 0x41 },
        {},
        { 0x72, 0x72, 0x41, 0x61 },
        {/**/ },
        {/**/ },
        { 0x00, 0x00, 0x00, 0x00 },
        { 0x00, 0x00, 0x55, 0xAA },
    };


    for (int i = 0; i < 480; i++) {
        default_FSInfo.first_reserved[i] = 0;
    }


    unsigned char* temp = reversed_int_to_char(quantity_empty_sectors);
    memcpy((void*)default_FSInfo.free_cluster_count, (void*)temp, 4);
    temp = reversed_int_to_char(begin_date);
    memcpy((void*)default_FSInfo.next_free_cluster, (void*)temp, 4);


    // дальше надо записать эту структуру в файл

    if (WriteFile(descriptor, &default_FSInfo, sizeof(struct FSInfo), NULL, NULL) == 0) {
        printf("\n Can not write FSInfo in Your device. \n");
        return false;
    }

    return true;
}



