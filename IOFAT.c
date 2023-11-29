#include"IOFAT.h"
#include"FATParse.h"

void IO_DisplayEntry(uint32_t entryAddress)
{
    Directory_Entry_Type Entry;
    uint16_t idx;
    FAT_Time_Type time;
    FAT_Date_Type date;

    Entry = FAT_ReadEntry(entryAddress);

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
    printf("FileSize: 0x%x\n", Entry.FileSize);
 
    // => cluster number
    printf("FirstCluster: 0x%x\n", Entry.FirstCluster);
}

void IO_DisplayDataCluster(uint16_t cluster)
{
    uint8_t data[0x200 + 1];
    uint32_t address;

    address = (33 + cluster) * 0x200;                   // ??? 33
    FAT_fseek(address);
    fgets(data, 0x200 + 1, FAT_GetFilePtr());
    printf("%s\n", data);
}

void IO_DisplayFile(uint16_t firstCluster)
{
    uint16_t cluster = firstCluster;

    while(cluster != 0xFFF){
        IO_DisplayDataCluster(cluster);
        cluster = FAT_GetNextCluster(cluster);
    }
} 