#include<stdint.h>
#include<stdio.h>
#include<math.h>
#include"FATParse.h"

uint8_t main(void){
    // root directory: 0x2600
    FAT_Status_Type status;

    status = FAT_OpenFile("floppy.img");
    if(FAT_FILE_EXIST == status){
        FAT_ReadBootBlock();
        FAT_ReadRootDirectory();

        // FAT_fseek(0x2600);
        // FAT_Get();


        FAT_CloseFile();
    }

    return 0;
}