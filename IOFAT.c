#include"IOFAT.h"

extern Directory_Entry_Type entryArray[];                 
extern uint32_t len;    

void IO_DisplayDataCluster(uint16_t cluster)
{
    uint8_t data[NUMBER_OF_BYTES_PER_BLOCK + 1];
    uint32_t address;

    address = (cluster + STARTING_DATA_AREA) * NUMBER_OF_BYTES_PER_BLOCK;   // (33 + cluster - 2) * 0x200
    FAT_fseek(address);
    fgets(data, NUMBER_OF_BYTES_PER_BLOCK + 1, FAT_GetFilePtr());
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

void IO_DisplayCurDir()                 // folder content: files, subdirs
{
    uint32_t idx, i;
    Directory_Entry_Type Entry;
    FAT_Time_Type time;
    FAT_Date_Type date;
    uint8_t *space = " ";
    uint8_t buff[30];

    // => title: fat12, name, type, size, date, time
    printf("                      FILE SYSTEM TYPE: %s\n", FAT_GetBootBlock().FileSystemType);
    printf("----------------------------------------------------------------------\n");
    printf("\n");
    printf("\n");
    printf("\n");
    // printf("|Name|%10s|Type|%10s|Size|%10s|Date|%10s|Time|\n", space, space, space, space);
    printf("|Name|%24s|Type|%24s|Size|%24s|Date|%24s|Time|\n", space, space, space, space);
    for(idx = 0; idx < len; idx++){
        Entry = entryArray[idx];

        // => index                                 // ocupied 4 chars
        // number of files inside is unknown, consider max size = 100
        printf("%2d", idx);
        buff[0] = '.';
        buff[1] = ' ';                 
        
        // => file name                             // 8
        for(i = 0; i < 8; i ++){
            buff[2 + i] = Entry.FileName[i];
        }
        buff[2 + i] = '\0';
        printf("%-28s", buff);                      // 30 - 2

        // => type                                  
        if(Entry.FileAttributes == 0x10){           // 6
            buff[0] = 'F';
            buff[1] = 'o';
            buff[2] = 'l';
            buff[3] = 'd';
            buff[4] = 'e';
            buff[5] = 'r';
            buff[6] = '\0';
        }
        else{                                        // 3
            for(i = 0; i < 3; i ++){
                buff[i] = Entry.FileNameExtension[i];
            }
            buff[i] = '\0';
        }
        printf("%-30s", buff);

        // => size
        if(Entry.FileAttributes == 0x00){
            printf("%-30d", Entry.FileSize);
        }
        else{
            printf("%30s", space);
        }

        // => date
        date.Day = Entry.ModifiedDate % (uint16_t)pow(2, 5);
        Entry.ModifiedDate = Entry.ModifiedDate >> 5;
        date.Month = Entry.ModifiedDate % (uint16_t)pow(2, 4);
        Entry.ModifiedDate = Entry.ModifiedDate >> 4;
        date.Year = Entry.ModifiedDate % (uint16_t)pow(2, 7);
        printf("%02d/%02d/%04d", date.Day, date.Month, date.Year + BASE_YEAR);              // 10
        printf("%20s", space);                                                              // 30 - 10

        // => time
        time.Second = (Entry.ModifiedTime % (uint16_t)pow(2, 5)) * 2;
        Entry.ModifiedTime = Entry.ModifiedTime >> 5;
        time.Minute = Entry.ModifiedTime % (uint16_t)pow(2, 6);
        Entry.ModifiedTime = Entry.ModifiedTime >> 6;
        time.Hour = Entry.ModifiedTime % (uint16_t)pow(2, 5);
        printf("%02d:%02d:%02d", time.Hour, time.Minute, time.Second);

        printf("\n");
    }
}

void IO_SolveUserInput(uint32_t rootDirAddr)
{
    uint8_t userIn;
    uint32_t dirAddr;
    Directory_Entry_Type Entry;
    FAT_Bool_Type isSubDir = FALSE;             // default is root dir

    dirAddr = rootDirAddr;
    while(1)
    {
        // => read entry
        FAT_ReadCurDirectory(dirAddr);
        IO_DisplayCurDir();

        // => enter user input
        if(isSubDir){
            printf("\n\n 0. Back\n");
        }else{
            printf("\n\n 0. Exit\n");
        }
        printf("Pls enter your number: ");
        // => userIn must be valid: >= 0 && <= len
        do
        {
            scanf("%hhd", &userIn);
        } while (!(userIn >= 0 && userIn <= len));

        if(userIn == 0)
        {
            if(isSubDir)
            {
                // =>back to previous dir <=> userIn = 2 & Entry.FileAttributes = 0x10
                userIn = 2;
                Entry = entryArray[userIn - 1];
                if(Entry.FirstCluster == 0x00){
                    isSubDir = FALSE;
                    dirAddr = rootDirAddr;
                }else{
                    isSubDir = TRUE;
                    dirAddr = FAT_GetSubDirAdd(Entry.FirstCluster);
                }
            }
            else
            {
                // =>Exit
                break;
            }
        }
        else if(userIn >= 1 && userIn <= len)
        {
            Entry = entryArray[userIn - 1];
            if(Entry.FileAttributes == 0x10)
            {
                if(Entry.FirstCluster == 0x00){
                    isSubDir = FALSE;
                    dirAddr = rootDirAddr;
                }else{
                    isSubDir = TRUE;
                    dirAddr = FAT_GetSubDirAdd(Entry.FirstCluster);
                }
            }   
            else
            {
                IO_DisplayFile(Entry.FirstCluster); 
                // => enter 0 to back
                printf(" 0. Back\n");
                printf("Pls enter your number: ");
                // => userIn must be 0
                do
                {
                    scanf("%hhd", &userIn);
                } while (userIn != 0);
            }
        }
    }
}