#ifndef _FATPARSE_H_
#define _FATPARSE_H_

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#pragma pack(1)                         // set up allignment = 1

typedef enum{
    FAT_FILE_NOT_EXIST,
    FAT_FILE_EXIST,
}FAT_Status_Type;

typedef struct
{
    /* data */
    uint8_t FileSystemType[9];              // including '\0'
    uint8_t NumberOfFATs;
    uint16_t SizeOfFAT;
    uint16_t NumberOfBytesPerBlock;
    uint32_t RootDirectoryAddress;          // output
}Boot_Block_Type;

// typedef struct{
//     uint8_t Directory_Entry[0x20];
// }Directory_Entry_Type;

typedef struct
{
    /* data */
    uint8_t FileName[8];
    uint8_t FileNameExtension[3];
    uint8_t FileAttributes;
    uint8_t Reserved[10];
    uint16_t FileTime;
    uint16_t FileDate;
    uint16_t StartingClusterNumber;
    uint32_t FileSize;
}Directory_Entry_Type;

typedef struct
{
    /* data */
    uint8_t year;
    uint8_t month;
    uint8_t day;
}FAT_File_Date_Type;



FAT_Status_Type FAT_OpenFile(const uint8_t* fileName);
void FAT_CloseFile(void);
void FAT_fseek(int64_t position);
void FAT_Get();
void FAT_ReadBootBlock();
void FAT_ReadRootDirectory();

#endif