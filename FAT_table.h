#include "Boot_sector.h"

bool set_default_FAT_table(HANDLE descriptor) {

    struct FAT_table {
        unsigned char table[24];
    }default_table;



    for (int i = 1; i <= (6 * 4); i++) {
        if (i == 1) {
            default_table.table[i - 1] = 0xf8;
            continue;
        }
        if (i == 8) {
            default_table.table[i - 1] = 0xff;
            continue;
        }
        if ((i % 4) == 0) {
            default_table.table[i - 1] = 0x0f;
        }
        else {
            default_table.table[i - 1] = 0xff;
        }

    }
    // ������ ���� �������� ��� ��������� � ����

    if (WriteFile(descriptor, &default_table, sizeof(struct FAT_table), NULL, NULL) == 0) {
        printf("\n Can not write FAT table in Your device. \n");
        return false;
    }


    return true;
}
