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
typedef enum
{
    FALSE,
    TRUE,
}FAT_Bool_Type;

typedef struct
{
    uint8_t FileSystemType[9];              // including '\0'
    uint8_t NumberOfFATs;
    uint16_t SizeOfFAT;
    uint16_t NumberOfBytesPerBlock;
    uint32_t RootDirectoryAddress;          // output
}Boot_Block_Type;

typedef struct
{
    uint8_t FileName[8];
    uint8_t FileNameExtension[3];
    uint8_t FileAttributes;
    uint16_t Reserved;
    uint16_t CreationTime;
    uint16_t CreationDate;
    uint16_t AccessDate;
    uint16_t HightOrder2Bytes;
    uint16_t ModifiedTime;
    uint16_t ModifiedDate;
    uint16_t FirstCluster;      // LowOrder2Bytes
    uint32_t FileSize;
}Directory_Entry_Type;

typedef struct 
{
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
}FAT_Time_Type;

typedef struct 
{
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;
}FAT_Date_Type;



FAT_Status_Type FAT_OpenFile(const uint8_t* fileName);
void FAT_CloseFile(void);
void FAT_fseek(int64_t position);
void FAT_Get();
uint8_t FAT_isOutOfFile(uint32_t directoryAddress);
uint32_t FAT_ReadBootBlock();
Directory_Entry_Type FAT_ReadEntry(uint32_t entryAddress);
void FAT_ReadRootDirectory(uint32_t rootDirectoryAddress);
void FAT_DisplayDataCluster(uint16_t cluster);
uint16_t FAT_GetNextCluster(uint16_t cluster);
void FAT_DisplayData(uint16_t firstCluster);

#endif