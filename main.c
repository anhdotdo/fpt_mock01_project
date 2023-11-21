#include<stdint.h>
#include<stdio.h>
#include<math.h>

uint8_t main(void){
    uint8_t buffer[9];
    uint8_t FileSystemType[9];
    uint8_t NumberOfFATs;
    uint16_t SizeOfFAT;
    uint32_t RootDirectoryAddress;

    // => get file system type
    FILE *fPtr = fopen("floppy.img", "r");
    fseek(fPtr, 0x36, SEEK_SET);
    fgets(FileSystemType, 9, fPtr);
    printf("%s\n", FileSystemType);
    // => get the root directory address
    fseek(fPtr, 0x10, SEEK_SET);
    fgets(buffer, 2, fPtr);
    NumberOfFATs = buffer[0];
    printf("%d\n", NumberOfFATs);

    fseek(fPtr, 0x16, SEEK_SET);
    fgets(buffer, 3, fPtr);
    SizeOfFAT = buffer[0] + buffer[1] * 16;
    printf("%d\n", SizeOfFAT);
    RootDirectoryAddress = (NumberOfFATs * SizeOfFAT + 1) * 0x200;
    printf("%x\n", RootDirectoryAddress);


    fclose(fPtr);
    return 0;
}