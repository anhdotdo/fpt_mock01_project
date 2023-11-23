#include<stdint.h>
#include<stdio.h>
#include<math.h>
#include"FATParse.h"

uint8_t main(void){
    // root directory: 0x2600
    FAT_Status_Type status;
    uint32_t rootDirectoryAddress;
    uint32_t directoryAddress;
    uint8_t idx;

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
        

        // FAT_ReadRootDirectory(rootDirectoryAddress);
        // FAT_ReadRootDirectory(rootDirectoryAddress + 0x20);

        // FAT_fseek(0x2600);
        // FAT_Get();


        FAT_CloseFile();
    }

    return 0;
}