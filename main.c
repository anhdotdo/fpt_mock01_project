#include<stdint.h>
#include<stdio.h>
#include<math.h>
#include"FATParse.h"

uint8_t main(void){
    // root directory: 0x2600
    FAT_Status_Type status;
    uint32_t rootDirectoryAddress;

    status = FAT_OpenFile("floppy.img");
    if(FAT_FILE_EXIST == status){
        rootDirectoryAddress = FAT_ReadBootBlock();
        
        // FAT_ReadRootDirectory(rootDirectoryAddress);
        // FAT_ReadRootDirectory((0x4f2 - 2 + 33) * 0x200);
        // FAT_DisplayDataCluster(0xcaf0);
        FAT_DisplayData(0x2eb);
        // printf("0x%x\n", FAT_GetNextCluster(0x026E, 1));
        // printf("0x%x\n", FAT_GetNextCluster(0x09));

        // FAT_fseek(0x2700);
        // FAT_Get();


        FAT_CloseFile();
    }

    return 0;
}