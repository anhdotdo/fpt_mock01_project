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



// static Node *HEAD = NULL;
// static uint32_t len = 0;

uint32_t FAT_GetLen();

FAT_Status_Type FAT_OpenFile(const uint8_t* fileName);              // open fat file
void FAT_CloseFile(void);                                           // close fat file
void FAT_fseek(int64_t position);                                   // move to a specific address
FILE *FAT_GetFilePtr();
uint32_t FAT_ReadBootBlock();                                       // read boot block
Directory_Entry_Type FAT_ReadEntry(uint32_t entryAddress);          // read a directory entry
void FAT_ReadCurDirectory(uint32_t rootDirectoryAddress);
uint16_t FAT_GetNextCluster(uint16_t cluster);                      // find the next data cluster
uint32_t FAT_GetSubDirAdd(uint16_t firstCluster);

#endif