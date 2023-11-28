#include<stdint.h>
#include<stdio.h>
#include<math.h>
#include"FATParse.h"

uint8_t main(void){
    // root directory: 0x2600
    FAT_Status_Type status;
    uint32_t rootDirectoryAddress;
    uint32_t entryAddress;

    uint16_t idx;
    uint16_t dataBlock;

    status = FAT_OpenFile("floppy.img");
    if(FAT_FILE_EXIST == status){
        rootDirectoryAddress = FAT_ReadBootBlock();
        entryAddress = rootDirectoryAddress;
        
        // FAT_ReadRootDirectory(rootDirectoryAddress);
        // FAT_DisplayDataCluster(0x49);
        FAT_DisplayData(0x500);
        // printf("0x%x\n", FAT_GetNextCluster(0x026E, 1));


        FAT_CloseFile();
    }

    return 0;
}