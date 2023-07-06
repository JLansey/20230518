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
#include "Led.h"
#include <stdlib.h>

//global variables
uint16_t SwitchOldTick;
uint8_t SwitchHornDebounce;
uint8_t SwitchHornStatus;


//Bell related
//uint16_t SWITCH_BELL_DELAY;  // how long should the bell ring for?
uint32_t SwitchBellCnt; // total count for the delay

uint32_t bell_flip_cnt;

uint32_t bell_t_on;
uint32_t bell_t_off;

uint32_t BellCntOn;
uint32_t BellCntOff;

uint8_t SwitchBellDingStatus; // are we on or off
uint8_t SwitchBellStatus; // is it bell ringing time?

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
	SwitchHornDebounce = SWITCH_HORN_DEBOUNCE_INITIAL; // 29
	SwitchHornStatus = 0;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : BellInit()
//* Object              : Set up user switch input
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void BellInit(void)
{
	//maybe don't need this function at all??
	
	//Initialize variables used for Bell Switch
	SwitchBellCnt = 0;	
	BellCntOn = 0;
	BellCntOff = 0;
	SwitchBellDingStatus = 0;
	SwitchBellStatus = 0;

}

//*--------------------------------------------------------------------------------------
//* Function Name       : TurnBellOn()
//* Object              : Turn on the bell
void TurnBellOn(void)
{
	SwitchBellStatus = 1;
	SwitchBellDingStatus = 1;
	SwitchBellCnt = SWITCH_BELL_DELAY;
	bell_t_on = BELL_T_ON;
	bell_t_off = BELL_T_OFF;
	BellCntOn = BELL_T_ON;

	bell_flip_cnt = 0;
	
	BellCntOff = 0; // it will be on for one cycle then switch off
	LED_Green(1);
	//blinkerG(2);
			
}
//*--------------------------------------------------------------------------------------
//* Function Name       : TurnBellOff()
//* Object              :Turn the bell off

void TurnBellOff(void)
{
	SwitchBellCnt = 0;
	BellCntOn = 0;

	SwitchBellStatus = 0;
	SwitchBellDingStatus = 0;
	LED_Green(0);
	LED_Red(0);

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
				TurnBellOff();                   // turn off the bell
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
				TurnBellOn();
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


//*--------------------------------------------------------------------------------------
//* Function Name       : BellUpdate()
//* Object              : update switch function
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void BellUpdateSwitch(void)
{
	//***********************************
	// Bell
	//***********************************
	//is switch off AND also are we still ringing the bell?
	if((!SwitchHornStatus) && SwitchBellStatus)
	{

		//drive the horn to make the bell sound
		if (BellCntOn > 0)
		{
			BellCntOn--;
		}

		if (BellCntOff > 0)
		{
			BellCntOff--;
		}

		// flip the speaker from on to off
		if(BellCntOn == 0 && SwitchBellDingStatus == 1)
		{
			BellCntOff = bell_t_off;
			SwitchBellDingStatus = 0;
			if (SwitchBellCnt < 90000)
			{
				if (bell_flip_cnt % 100 < 33){
					bell_t_off = 15;
					//LED_Red(1);
				}
				else
				{
					if (bell_flip_cnt % 100 > 66) bell_t_off = 20;
					else bell_t_off = 10;
				}
			}
		}

		if(BellCntOff == 0 && SwitchBellDingStatus == 0)
		{
			//blinker(3);
			BellCntOn = bell_t_on;
			SwitchBellDingStatus = 1;
			bell_flip_cnt++;
		}	



//regular turn entire bell process on or off
		if(SwitchBellCnt > 0)
		{
			SwitchBellCnt--;
		}
		if(SwitchBellCnt == 0)
		{
			TurnBellOff();
		}

	}
}


//*--------------------------------------------------------------------------------------
//* Function Name       : SwitchBellGetStatus()
//* Object              : return the status of the horn switch
//* Input Parameters    : none
//* Output Parameters   : uint8_t = horn switch Status
//*--------------------------------------------------------------------------------------

uint8_t SwitchBellGetStatus(void)
{
	return SwitchBellStatus;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : SwitchClearBellStatus()
//* Object              : clear bell bit
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void SwitchClearBellStatus(void)
{
	SwitchBellStatus = 0;
}


uint8_t GetBellSpeakerStatus(void)
{
	if (BellCntOn > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}



