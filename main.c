#include<stdint.h>
#include<stdio.h>
#include<math.h>
#include"FATParse.h"
#include"IOFAT.h"

uint8_t main(void){
    // root directory: 0x2600
    FAT_Status_Type status;
    uint32_t rootDirectoryAddress;
    uint32_t lengthOfCurrentDir;
    uint8_t userInput;

    status = FAT_OpenFile("floppy.img");
    if(FAT_FILE_EXIST == status){
        rootDirectoryAddress = FAT_ReadBootBlock();

        FAT_ReadRootDirectory(rootDirectoryAddress);
        lengthOfCurrentDir = FAT_GetLen();
        IO_DisplayDir();

        do
        {
            printf(" 0. Exit\n");
            printf("Pls enter your number: ");
            scanf("%hhd", &userInput);

            if(userInput >= 1 && userInput <= lengthOfCurrentDir)
            {
                    IO_SolveUserInput(userInput);
            }

        } while (userInput != 0);
        
        
        // FAT_ReadRootDirectory(rootDirectoryAddress);
        // IO_DisplayDir();

        // FAT_ReadRootDirectory((0x4f2 - 2 + 33) * 0x200);
        // FAT_DisplayDataCluster(0xcaf0);
        // FAT_DisplayData(0x02);
        // printf("0x%x\n", FAT_GetNextCluster(0x026E, 1));
        // printf("0x%x\n", FAT_GetNextCluster(0x09));

        // IO_DisplayFile(0x2eb);

        // FAT_ReadRootDirectory(rootDirectoryAddress);
        // printf("%d\n", FAT_GetLen());    

        FAT_CloseFile();
    }

    return 0;
}