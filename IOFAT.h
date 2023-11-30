#ifndef _IOFAT_H_
#define _IOFAT_H_

#include"stdio.h"
#include"stdint.h"
#include"FATParse.h"

void IO_DisplayDataCluster(uint16_t cluster);       // display a data cluster
void IO_DisplayFile(uint16_t firstCluster);         // display all data cluster
void IO_DisplayCurDir();
void IO_SolveUserInput(uint32_t rootDirAddr);

#endif