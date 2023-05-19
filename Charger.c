/*****************************************************************************************
**
**  Led.c
**
**  LED Functions for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

#include <avr/io.h>
#include "Charger.h"


//*--------------------------------------------------------------------------------------
//* Function Name       : Charger_init()
//* Object              : setup I/O pins used by charger IC
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void Charger_init(void)
{
	CHARGER_PWR_GOOD_PORT.DIRCLR = CHARGER_PWR_GOOD_BIT;
	CHARGER_PWR_GOOD_PORT.OUTSET = CHARGER_PWR_GOOD_BIT;
	CHARGER_PWR_GOOD_CTRL = PORT_PULLUPEN_bm;
	
	CHARGER_STATUS_PORT.DIRCLR = CHARGER_STATUS_BIT;
	CHARGER_STATUS_PORT.OUTSET = CHARGER_STATUS_BIT;
	CHARGER_STATUS_CTRL = PORT_PULLUPEN_bm;
}
