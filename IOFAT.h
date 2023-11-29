#ifndef _IOFAT_H_
#define _IOFAT_H_

#include"stdio.h"
#include"stdint.h"
#include"FATParse.h"

void IO_DisplayEntry(uint32_t entryAddress);        // display directory entrys
void IO_DisplayDataCluster(uint16_t cluster);       // display a data cluster
void IO_DisplayFile(uint16_t firstCluster);         // display all data cluster
void IO_DisplayDir();
void IO_SolveUserInput(uint8_t userInput);

#endif