/*****************************************************************************************
**
**  Horn.c
**
**  Horn Functions for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

#include <avr/io.h>
#include "Horn.h"

//*--------------------------------------------------------------------------------------
//* Function Name       : Horn_init()
//* Object              : setup I/O pins used by horn
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void Horn_init(void)
{
	HORN_PORT.OUTCLR = HORN_BIT;
	HORN_PORT.DIRSET = HORN_BIT;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : Horn_Enable()
//* Object              : setup I/O pins used by horn
//* Input Parameters    : uint8_t Enable = true to turn horn on
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void Horn_Enable(uint8_t Enable)
{
	if(Enable)
	{
		HORN_PORT.OUTSET = HORN_BIT;
	}
	else
	{
		HORN_PORT.OUTCLR = HORN_BIT;
	}
}
