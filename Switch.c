/*****************************************************************************************
**
**  Switch.c
**
**  Switch Functions
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

#include <avr/io.h>
#include "Timer.h"
#include "Switch.h"

//global variables
uint16_t SwitchOldTick;
uint8_t SwitchHornDebounce;
uint8_t SwitchHornStatus;


//*--------------------------------------------------------------------------------------
//* Function Name       : SwitchInit()
//* Object              : Set up user switch input
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void SwitchInit(void)
{
	//Configure ID Pins
	SWITCH_HORN_PORT.DIRCLR = SWITCH_HORN_BIT;
	SWITCH_HORN_PORT.OUTSET = SWITCH_HORN_BIT;
	SWITCH_HORN_CTRL = PORT_PULLUPEN_bm;

	//Initialize variables used for Horn Switch
	SwitchOldTick = 0;
	SwitchHornDebounce = 0;
	SwitchHornStatus = 0;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : SwitchUpdate()
//* Object              : update switch function
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void SwitchUpdate(void)
{
	//check if time to scan switches
	if(RTC_getTick() != SwitchOldTick)
	{
		SwitchOldTick = RTC_getTick();

		//***********************************
		// Horn Switch
		//***********************************
		//is switch pressed?
		if(!(SWITCH_HORN_PORT.IN & SWITCH_HORN_BIT))
		{
			if(SwitchHornDebounce < 255)
			{
				SwitchHornDebounce++;
			}
			if(SwitchHornDebounce == TIME_SWITCH_PRESS_DET)
			{
				SwitchHornDebounce = 255;				//indicate switch pressed
				SwitchHornStatus = 1;					//sets that a switch has been pressed
			}
		}
		else
		{
			//Switch not pressed
			if(SwitchHornDebounce > 0)
			{
				SwitchHornDebounce--;
			}
			if(SwitchHornDebounce == (255 - TIME_SWITCH_RELEASE_DET))
			{
				SwitchHornDebounce = 0;
				SwitchHornStatus = 0;					//clear bit indicating horn switch no longer pressed
			}
		}
	}
}


//*--------------------------------------------------------------------------------------
//* Function Name       : SwitchHornGetStatus()
//* Object              : return the status of the horn switch
//* Input Parameters    : none
//* Output Parameters   : uint8_t = horn switch Status
//*--------------------------------------------------------------------------------------

uint8_t SwitchHornGetStatus(void)
{
	return SwitchHornStatus;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : SwitchClearHornStatus()
//* Object              : clear horn bit
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void SwitchClearHornStatus(void)
{
	SwitchHornStatus = 0;
}
