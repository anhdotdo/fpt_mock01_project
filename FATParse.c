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
    fgets((char*)&Entry, sizeof(Directory_Entry_Type), fPtr);

    // printf("-----%x\n", Entry.FileAttributes);
    printf("-------%x\n", Entry.StartingClusterNumber);
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
void FAT_ReadDirectoryEntry(uint32_t directoryEntry){
    uint8_t idx;
    uint8_t *pU8 = NULL;
    uint8_t *pDirecEntry = (uint8_t*)malloc(sizeof(Directory_Entry_Type));
    uint8_t firstHexNumber, secondHexNumber;
    uint16_t u16_temp;
    FAT_File_Date_Type fileDate;

    // variable for finding data blocks
    uint8_t buff[9];
    uint16_t maxEntriesOfRoot;
    uint16_t blocksRootOccupied;
    uint8_t offsetConvertClusterNumber2BlockNumber;
    uint8_t firstDataBlock;
    uint8_t firstDataBlockEntry;
    uint8_t offsetFirstDataBlockEntry;

    FAT_fseek(directoryEntry);
    fgets(pDirecEntry, sizeof(Directory_Entry_Type), fPtr);
    Directory_Entry_Type* pTemp = (Directory_Entry_Type*)pDirecEntry;

    // => file name
    printf("------------\n");
    for(idx = 0; idx < 8; idx ++){
        // if(' ' == pTemp->FileName[idx]){
        //     break;
        // }
        printf("%c", pTemp->FileName[idx]);
    }

    // => file extention
    printf(".");
    for(idx = 0; idx < 3; idx ++){
        printf("%c", pTemp->FileNameExtension[idx]);
    }

    // => file attribute
    printf("\nFileAttributes: %x", pTemp->FileAttributes);

    // => file date
    // uint16_t u16_temp;
    // FAT_File_Date_Type fileDate;

    printf("\nFileDate: %x", pTemp->FileDate);
    pU8 = (uint8_t*)&pTemp->FileDate;
    pTemp->FileDate = ((uint16_t)pU8[0] << 8) + ((uint16_t)pU8[1] << 0);
    printf("\nFileDate fixed: %x", pTemp->FileDate);
    u16_temp = pTemp->FileDate;
    fileDate.day = u16_temp % (uint16_t)pow(2, 5);
    printf("\nDay: %d", fileDate.day);
    u16_temp /= (uint16_t)pow(2, 5);
    fileDate.month = u16_temp % (uint16_t)pow(2, 4);
    printf("\nMonth: %d", fileDate.month);
    u16_temp /= (uint16_t)pow(2, 4);
    fileDate.year = u16_temp;
    printf("\nYear: %d", fileDate.year);

    printf("\n");

    // => if is not folder -> return 
    if(pTemp->FileAttributes != 0x10){
        return;
    }



    // => starting cluster number
    printf("\nStartingClusterNumber: %x", pTemp->StartingClusterNumber);
    pU8 = (uint8_t*)&pTemp->StartingClusterNumber;
    pTemp->StartingClusterNumber = ((uint16_t)pU8[0] << 8) + ((uint16_t)pU8[1] << 0);
    printf("\nStartingClusterNumber fixed: %x", pTemp->StartingClusterNumber);

    // => file size
    printf("\nFileSize: %x", pTemp->FileSize);
    pU8 = (uint8_t*)&pTemp->FileSize;
    pTemp->FileSize = ((uint32_t)pU8[0] << 24U) + ((uint32_t)pU8[1] << 16U) + ((uint32_t)pU8[2] << 8U) + ((uint32_t)pU8[4] << 0U);
    printf("\nFileSize fixed: %x", pTemp->FileSize);

    printf("\n");

    /*
    first cluster is cluster: 0x300
    root directory at block: 19, address: 0x2600
    */
    // => maximum number of entries in the root directory
    FAT_fseek(0x11);
    fgets(buff, 3, fPtr);
    printf("\n%02x-%02x", buff[0], buff[1]);
    u16_temp = buff[0] + buff[1] * (uint16_t)pow(2, 8);
    maxEntriesOfRoot = u16_temp;
    printf("\nmaxEntriesOfRoot: %04x", u16_temp);
    // => number of blocks occupied by the root directory
    blocksRootOccupied = (maxEntriesOfRoot * 0x20) >> 9;
    printf("\nblocksRootOccupied: %d", blocksRootOccupied);
    // => convert a cluster number (which is what appears in the root directory) to a block number
    offsetConvertClusterNumber2BlockNumber = (bootBlock.RootDirectoryAddress >> 9) + blocksRootOccupied - 2;
    printf("\noffsetConvertClusterNumber2BlockNumber: %x", offsetConvertClusterNumber2BlockNumber);
    // => the first data block of the file
    // block: pTemp->StartingClusterNumber + offsetConvertClusterNumber2BlockNumber;
    firstDataBlock = pTemp->StartingClusterNumber + offsetConvertClusterNumber2BlockNumber;
    printf("\nfirstDataBlock: %x", firstDataBlock);
    // => the entry for the first cluster
    // pTemp->StartingClusterNumber = 0xf4e;                        // test calculator => 0x10, 0x09
    firstDataBlockEntry = 1 + ((pTemp->StartingClusterNumber * 2) >> 9);
    offsetFirstDataBlockEntry = pTemp->StartingClusterNumber * 2 % 0x200;
    printf("\nfirstDataBlockEntry: %x", firstDataBlockEntry);
    printf("\noffsetFirstDataBlockEntry: %x", offsetFirstDataBlockEntry);

    printf("\n");
}

/* Task 03: manage directory and display
input:
output:
*/