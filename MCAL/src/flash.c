/*
 * flash.c
 *
 *  Created on: Nov 24, 2020
 *      Author: MahmoudH
 */


#include "flash.h"

void FLASH_MainSectorInit(void){
	FLASH_Unlock();
	FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3);
	FLASH_Lock();
}
