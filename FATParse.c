#include"FATParse.h"

// macro for boot block
#define FILE_SYSTEM_TYPE 0x36                                           
#define FILE_SYSTEM_TYPE_LENGTH 8
#define NUMBER_OF_FATS 0x10
#define NUMBER_OF_FATS_LENGTH 1
#define SIZE_OF_FAT 0x16
#define SIZE_OF_FAT_LENGTH 2
#define NUMBER_OF_BYTES_PER_BLOCK 0x0b
#define NUMBER_OF_BYTES_PER_BLOCK_LENGTH 2

static FILE *fPtr = NULL;                                               // global variable
static Boot_Block_Type bootBlock;

Directory_Entry_Type entryArray[100];                 
uint32_t len = 0;          

// => get global variable
uint32_t FAT_GetLen()
{
    return len;
}

// function to open file                                                // prototype
FAT_Status_Type FAT_OpenFile(const uint8_t* fileName){
    FAT_Status_Type status = FAT_FILE_EXIST;
    fPtr = fopen(fileName, "r");
    if(fPtr == NULL){
        status = FAT_FILE_NOT_EXIST;
    }
    return status;
}

// function to close file
void FAT_CloseFile(void){
    // free(srec_line.Data);
    fclose(fPtr);
}

void FAT_fseek(int64_t position){
    fseek(fPtr, position, SEEK_SET);
}

FILE *FAT_GetFilePtr()
{
    return fPtr;
}

/* Task 01: boot block
input: floppy.img file
output: FAT type, find root
*/
uint32_t FAT_ReadBootBlock(){
    uint8_t buffer[9];
    FAT_fseek(FILE_SYSTEM_TYPE);
    fgets(bootBlock.FileSystemType, FILE_SYSTEM_TYPE_LENGTH+1, fPtr);
    // printf("%s\n", bootBlock.FileSystemType);
    FAT_fseek(NUMBER_OF_FATS);
    fgets(buffer, NUMBER_OF_FATS_LENGTH+1, fPtr);
    bootBlock.NumberOfFATs = buffer[0];
    // printf("%d\n", bootBlock.NumberOfFATs);
    FAT_fseek(SIZE_OF_FAT);
    fgets(buffer, SIZE_OF_FAT_LENGTH+1, fPtr);
    bootBlock.SizeOfFAT = buffer[0] + buffer[1]*16*16;
    // printf("%d\n", bootBlock.SizeOfFAT);
    FAT_fseek(NUMBER_OF_BYTES_PER_BLOCK);
    fgets(buffer, NUMBER_OF_BYTES_PER_BLOCK_LENGTH+1, fPtr);
    bootBlock.NumberOfBytesPerBlock = buffer[0] + buffer[1]*16*16;
    // printf("%d\n", bootBlock.NumberOfBytesPerBlock);
    bootBlock.RootDirectoryAddress = (bootBlock.NumberOfFATs * bootBlock.SizeOfFAT + 1) * bootBlock.NumberOfBytesPerBlock;
    // printf("%x\n", bootBlock.RootDirectoryAddress);
    return bootBlock.RootDirectoryAddress;
}

/* Task 02: root directory
input: root directory address
output: read entry, read data
*/
Directory_Entry_Type FAT_ReadEntry(uint32_t entryAddress)
{
    Directory_Entry_Type Entry;
    FAT_fseek(entryAddress);
    fgets((char*)&Entry, sizeof(Directory_Entry_Type), fPtr);
    return Entry;
}

void FAT_ReadCurDirectory(uint32_t rootDirectoryAddress)
{
    Directory_Entry_Type Entry;
    uint32_t entryAddress = rootDirectoryAddress;
    uint32_t idx = 0;
    len = 0;

    FAT_fseek(entryAddress);
    Entry = FAT_ReadEntry(entryAddress);
    while (Entry.FileName[0] != 0x00)
    {
        if(Entry.FileName[0] == 0xE5){
            continue;
        }
        // => read dir entry and save in List
        if(Entry.FileAttributes != 0x0F){
            entryArray[idx] = Entry;
            idx++; 
            len++;
        }

        FAT_fseek(entryAddress += 0x20);
        Entry = FAT_ReadEntry(entryAddress);
    }
}

uint16_t FAT_GetNextCluster(uint16_t cluster)
{
    uint16_t nextCluster;
    uint8_t byteArr[3];             // [0]: previous byte, [1]: current byte
    uint32_t address;

    address = 0x200 + cluster * 1.5;
    FAT_fseek(address);           // ?? 0x200, 1,5
    fgets(byteArr, 3, fPtr);
    if(cluster % 2 == 0)                // low-order nibble
    {   
        nextCluster = byteArr[0] | (((uint16_t)byteArr[1] & 0xF) << 8);
    }
    else                                // high-order nibble
    {   
        nextCluster = ((byteArr[0] & 0xF0) >> 4) | (((uint16_t)byteArr[1] & 0xFF) << 4);
    }
    return nextCluster;
}

uint32_t FAT_GetSubDirAdd(uint16_t firstCluster)
{
    uint32_t address;
    address = (33 + firstCluster - 2) * 0x200;
    return address;
}



/* Task 03: manage directory and display
input:
output:
*/