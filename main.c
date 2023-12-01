#include<stdint.h>
#include<stdio.h>
#include<math.h>
#include"FATParse.h"
#include"IOFAT.h"

uint8_t main(void){
    FAT_Status_Type status;
    uint32_t rootDirectoryAddress;

    status = FAT_OpenFile("floppy.img");
    if(FAT_FILE_EXIST == status){
        rootDirectoryAddress = FAT_ReadBootBlock();

        IO_SolveUserInput(rootDirectoryAddress);

        FAT_CloseFile();
    }

    return 0;
}