#include <fcntl.h>	/* open() and O_XXX flags */
#include <sys/stat.h>	/* S_IXXX flags */
#include <sys/types.h>	/* mode_t */
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <windows.h>
#include <winioctl.h>

#include "FSInfo.h"
#include "FAT_table.h"



int main(int argc, char const* argv[]) {

    const char* device_path = "F:/";

    HANDLE current_device_descriptor = open_device(device_path);

    char zero = 0;

    long int device_size = 0;


    // зануляем каждый байт и подсчитываем размер девайса
    while ((WriteFile(current_device_descriptor, &zero, sizeof(char), NULL, NULL)) != 0) {
        device_size++;
    }


    // сдвиг указателя в начало
    if (seek_descriptor(current_device_descriptor, 0) == false) {
        printf("\n Error in 1 seek \n");
        exit(0);
    }


    // проверка на изъятие
    if ((WriteFile(current_device_descriptor, &zero, sizeof(char), NULL, NULL)) == 0) {
        printf("\n Disconnection with device \n");
        exit(0);
    }


    // сдвиг указателя в начало
    if (seek_descriptor(current_device_descriptor, 0) == false) {
        printf("\n Error in 2 seek \n");
        exit(0);
    }

    int quantity_all_sectors = device_size / 512;
    int size_table_FAT_bute = quantity_all_sectors * 4;

    int  size_table_FAT_sector;

    if ((size_table_FAT_bute % 512) == 0) {
        size_table_FAT_sector = size_table_FAT_bute / 512;
    }
    else {
        size_table_FAT_sector = (size_table_FAT_bute / 512) + 1;
    }

    int begin_first_FAT = 32;

    int begin_second_FAT = begin_first_FAT + size_table_FAT_sector;

    int begin_date = begin_second_FAT + size_table_FAT_sector + 2;      // +2 потому что адресация нацинается со 2 сектора

    int quantity_empty_sectors = quantity_all_sectors - begin_date;
    int quantity_not_empty_sectors = begin_date;



    // записываем структуру Boot
    if (set_default_Boot_sector(current_device_descriptor, quantity_all_sectors, size_table_FAT_sector) == false) {
        printf("\n Error with setting Boot sector  \n");
        exit(0);
    }

    // записываем структуру FSInfo
    if (set_default_FSInfo(current_device_descriptor, quantity_empty_sectors, begin_date) == false) {
        printf("\n Error with setting FSInfo sector  \n");
        exit(0);
    }

    // сдвиг указателя на 6 секторов от начала файла
    if (seek_descriptor(current_device_descriptor, 6 * 512) == false) {
        printf("\n Error in 3 seek \n");
        exit(0);
    }

    // записываем бэкап структуры Boot
    if (set_default_Boot_sector(current_device_descriptor, quantity_all_sectors, size_table_FAT_sector) == false) {
        printf("\n Error with setting backup Boot sector  \n");
        exit(0);
    }

    // записываем бэкап структуры FSInfo
    if (set_default_FSInfo(current_device_descriptor, quantity_empty_sectors, begin_date) == false) {
        printf("\n Error with setting backup FSInfo sector  \n");
        exit(0);
    }

    // сдвиг указателя на 32 сектора от начала файла  (к началу FAT1)
    if (seek_descriptor(current_device_descriptor, begin_first_FAT * 512) == false) {
        printf("\n Error in 4 seek \n");
        exit(0);
    }

    // записываем FAT1
    if (set_default_FAT_table(current_device_descriptor) == false) {
        printf("\n Error with setting FAT1 sector  \n");
        exit(0);
    }

    // сдвиг указателя от начала файла на 32 сектора + size_table_FAT_sector  (к началу FAT2)
    if (seek_descriptor(current_device_descriptor, begin_second_FAT * 512) == false) {
        printf("\n Error in 5 seek \n");
        exit(0);
    }

    // записываем FAT2
    if (set_default_FAT_table(current_device_descriptor) == false) {
        printf("\n Error with setting FAT2 sector  \n");
        exit(0);
    }

    printf("\n Formatting was successfully!! \n");


    return 0;
}
