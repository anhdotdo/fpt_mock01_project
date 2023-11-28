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

void FAT_Get()
{
    Directory_Entry_Type Entry;
    FAT_fseek(0x2740);
    fgets((char*)&Entry, sizeof(Directory_Entry_Type), fPtr);

    printf("-----0x%x\n", Entry.FirstCluster);

    // => test
    uint8_t buff[9];
    FAT_fseek(0x2740 + 26);
    fgets(buff, 3, fPtr);
    printf("test: 0x%x, 0x%x\n", buff[0], buff[1]);
}

uint8_t FAT_isOutOfFile(uint32_t directoryAddress){
    FAT_fseek(directoryAddress);
    if(fgetc(fPtr) == 0x00){
        return 1;                   // true = out of file
    }
    return 0;
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
    uint16_t idx;
    FAT_Time_Type time;
    FAT_Date_Type date;
    uint8_t* ptrU8;

    FAT_fseek(entryAddress);
    fgets((char*)&Entry, sizeof(Directory_Entry_Type), fPtr);

    // => file name
    printf("------------\n");
    for(idx = 0; idx < 8; idx ++){
        // if(' ' == pTemp->FileName[idx]){
        //     break;
        // }
        printf("%c", Entry.FileName[idx]);
    }

    // => file extention
    printf(".");
    for(idx = 0; idx < 3; idx ++){
        printf("%c", Entry.FileNameExtension[idx]);
    }
    printf("\n");

    // => file attribute
    printf("FileAttributes: 0x%x\n", Entry.FileAttributes);

    // => file time (h-m-s:5-6-5)
    // FAT_Time_Type time;

    printf("CreationTime: 0x%x\n", Entry.CreationTime);
    time.Second = (Entry.CreationTime % (uint16_t)pow(2, 5)) * 2;
    Entry.CreationTime = Entry.CreationTime >> 5;
    time.Minute = Entry.CreationTime % (uint16_t)pow(2, 6);
    Entry.CreationTime = Entry.CreationTime >> 6;
    time.Hour = Entry.CreationTime % (uint16_t)pow(2, 5);
    printf("CreationTime fixed: %02d:%02d:%02d\n", time.Hour, time.Minute, time.Second);

    // => file date (y-m-d:7-4-5)
    // FAT_Date_Type date;

    // Entry.CreationDate = 0x3965;    // => 05/11/2008 test case
    printf("CreationDate: 0x%x\n", Entry.CreationDate);
    date.Day = Entry.CreationDate % (uint16_t)pow(2, 5);
    Entry.CreationDate = Entry.CreationDate >> 5;
    date.Month = Entry.CreationDate % (uint16_t)pow(2, 4);
    Entry.CreationDate = Entry.CreationDate >> 4;
    date.Year = Entry.CreationDate % (uint16_t)pow(2, 7);
    printf("CreationDate fixed: %02d:%02d:%04d\n", date.Day, date.Month, date.Year + 1980);

    // => modified time
    printf("ModifiedTime: 0x%x\n", Entry.ModifiedTime);
    time.Second = (Entry.ModifiedTime % (uint16_t)pow(2, 5)) * 2;
    Entry.ModifiedTime = Entry.ModifiedTime >> 5;
    time.Minute = Entry.ModifiedTime % (uint16_t)pow(2, 6);
    Entry.ModifiedTime = Entry.ModifiedTime >> 6;
    time.Hour = Entry.ModifiedTime % (uint16_t)pow(2, 5);
    printf("ModifiedTime fixed: %02d:%02d:%02d\n", time.Hour, time.Minute, time.Second);

    // => modified date
    printf("ModifiedDate: 0x%x\n", Entry.ModifiedDate);
    date.Day = Entry.ModifiedDate % (uint16_t)pow(2, 5);
    Entry.ModifiedDate = Entry.ModifiedDate >> 5;
    date.Month = Entry.ModifiedDate % (uint16_t)pow(2, 4);
    Entry.ModifiedDate = Entry.ModifiedDate >> 4;
    date.Year = Entry.ModifiedDate % (uint16_t)pow(2, 7);
    printf("ModifiedDate fixed: %02d:%02d:%04d\n", date.Day, date.Month, date.Year + 1980);

    // => file size
    // uint8_t* ptrU8;

    printf("FileSize: 0x%x\n", Entry.FileSize);
 
    // => cluster number
    printf("FirstCluster: 0x%x\n", Entry.FirstCluster);
    
    return Entry;
}

void FAT_ReadRootDirectory(uint32_t rootDirectoryAddress)
{
    Directory_Entry_Type Entry;
    uint32_t entryAddress = rootDirectoryAddress;

    FAT_fseek(entryAddress);
    Entry = FAT_ReadEntry(entryAddress);
    while (Entry.FileName[0] != 0x00)
    {
        if(Entry.FileName[0] == 0xE5){
            continue;
        }
        FAT_fseek(entryAddress += 0x20);
        // printf("entryAddress: 0x%x\n", entryAddress);
        Entry = FAT_ReadEntry(entryAddress);
    }
}

void FAT_DisplayDataCluster(uint16_t cluster)
{
    uint32_t address;
    uint8_t data[0x200 + 1];

    address = (33 + cluster) * 0x200;                   // ??? 33
    FAT_fseek(address);
    fgets(data, 0x200 + 1, fPtr);
    printf("%s\n", data);
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

void FAT_DisplayData(uint16_t firstCluster)
{
    uint16_t cluster = firstCluster;

    while(cluster != 0xFFF){
        FAT_DisplayDataCluster(cluster);
        cluster = FAT_GetNextCluster(cluster);
    }
} 

/* Task 03: manage directory and display
input:
output:
*/