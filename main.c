#include<stdint.h>
#include<stdio.h>
#include<math.h>
#include"FATParse.h"

uint8_t main(void){
    // root directory: 0x2600
    FAT_Status_Type status;
    uint32_t rootDirectoryAddress;
    uint32_t directoryAddress;
    uint16_t idx;
    uint16_t dataBlock;

    status = FAT_OpenFile("floppy.img");
    if(FAT_FILE_EXIST == status){
        rootDirectoryAddress = FAT_ReadBootBlock();
        directoryAddress = rootDirectoryAddress;
        while (1)
        {
            if(FAT_isOutOfFile(directoryAddress)){
                break;
            }
            FAT_ReadDirectoryEntry(directoryAddress);
            directoryAddress += 0x20;
        }

        // FAT_DisplayDataBlock(0x41F * 0x200, 0x05, 0x00);


        FAT_CloseFile();
    }

    return 0;
}