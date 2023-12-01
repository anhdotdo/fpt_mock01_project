#include"FATParse.h"

static FILE *fPtr = NULL;                                               // global variable
static Boot_Block_Type bootBlock;

Directory_Entry_Type entryArray[100];                                   // save entrys from one directory
uint32_t len = 0;                           

// => get global variable
FILE *FAT_GetFilePtr()
{
    return fPtr;
}

Boot_Block_Type FAT_GetBootBlock()
{
    return bootBlock;
}

// uint32_t FAT_GetLen()
// {
//     return len;
// }

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
    fclose(fPtr);
}

void FAT_fseek(int64_t position){
    fseek(fPtr, position, SEEK_SET);
}

/* Task 01: boot block
input: floppy.img file
output: FAT type, find root
*/
uint32_t FAT_ReadBootBlock(){
    uint8_t buffer[9];
    FAT_fseek(FILE_SYSTEM_TYPE_ADDR);
    fgets(bootBlock.FileSystemType, FILE_SYSTEM_TYPE_LENGTH+1, fPtr);                                                               // FAT12
    FAT_fseek(NUMBER_OF_FATS_ADDR);
    fgets(buffer, NUMBER_OF_FATS_LENGTH+1, fPtr);
    bootBlock.NumberOfFATs = buffer[0];
    FAT_fseek(SIZE_OF_FAT_ADDR);
    fgets(buffer, SIZE_OF_FAT_LENGTH+1, fPtr);
    bootBlock.SizeOfFAT = buffer[0] + buffer[1]*16*16;
    FAT_fseek(NUMBER_OF_BYTES_PER_BLOCK_ADDR);
    fgets(buffer, NUMBER_OF_BYTES_PER_BLOCK_LENGTH+1, fPtr);
    bootBlock.NumberOfBytesPerBlock = buffer[0] + buffer[1]*16*16;                                                                  // 0x200
    bootBlock.RootDirectoryAddress = (bootBlock.NumberOfFATs * bootBlock.SizeOfFAT + 1) * bootBlock.NumberOfBytesPerBlock;          // 0x2600
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

        FAT_fseek(entryAddress += NUMBER_OF_BYTES_PER_ENTRY);
        Entry = FAT_ReadEntry(entryAddress);
    }
}

uint16_t FAT_GetNextCluster(uint16_t cluster)
{
    uint16_t nextCluster;
    uint8_t byteArr[3];             // [0]: previous byte, [1]: current byte
    uint32_t address;

    address = STARTING_FAT_TABLES * NUMBER_OF_BYTES_PER_BLOCK + (cluster + (cluster >> 1));   // 0x200 + cluster * 1.5
    FAT_fseek(address);           
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
    address = (STARTING_DATA_AREA + firstCluster) * NUMBER_OF_BYTES_PER_BLOCK;  // (33 + firstCluster - 2) * 0x200
    return address;
}