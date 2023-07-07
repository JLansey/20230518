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
#include <stddef.h>
#include <math.h>

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

uint8_t BellCntOn;
uint32_t BellCntOff;

uint8_t SwitchBellDingStatus; // are we on or off
uint8_t SwitchBellStatus; // is it bell ringing time?

// Array for the pulse data
//uint64_t pulseCenters[PULSE_DATA_SIZE] = {0, 4, 8, 11};
uint64_t pulseCenters[PULSE_DATA_SIZE];
//uint64_t pulseCenters[PULSE_DATA_SIZE] = {0, 38, 76, 114};

//formant hz at 1000
//uint64_t pulseCenters[PULSE_DATA_SIZE] = {0, 4, 8, 11};

//uint64_t pulseCenters[PULSE_DATA_SIZE] = {0, 150, 0, 0};

uint32_t pulseWidths[PULSE_DATA_SIZE];

uint8_t expLookupPWM[98] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 1, 1, 1, 1};

//uint32_t pulseWidths[PULSE_DATA_SIZE] = {1, 1, 1, 1};
//uint32_t pulseWidths[PULSE_DATA_SIZE] = {5, 16, 20, 13};



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
	//bell_t_on = BELL_T_ON;
	bell_t_off = BELL_T_OFF;
	
	BellCntOn = 0;// BELL_T_ON;
    bell_t_on = 15;
	bell_flip_cnt = 0;
	
	//BellCntOff = 0; // it will be on for one cycle then switch off
	LED_Green(1);

//	uint32_t fixedPulseCentersValues[PULSE_DATA_SIZE] = {0, 8, 16, 22};
	//uint32_t fixedPulseWidthsValues[PULSE_DATA_SIZE] = {2, 5, 6, 4};

//	uint32_t fixedPulseCentersValues[PULSE_DATA_SIZE] = {0, 68, 136, 204};
	uint32_t fixedPulseCentersValues[PULSE_DATA_SIZE] = {0, 1, 7, 9, 10, 16, 18, 19, 26, 27, 29, 35};
	uint32_t fixedPulseWidthsValues[PULSE_DATA_SIZE] = {2, 3, 2, 8, 6, 3, 10, 5, 4, 6, 2, 1};
	
	for(int i = 0; i < PULSE_DATA_SIZE; i++)
	{
		pulseCenters[i] = fixedPulseCentersValues[i];
		pulseWidths[i] = fixedPulseWidthsValues[i];
	}

//	pulseCenters[PULSE_DATA_SIZE] = {0, 8, 16, 22};
	//pulseWidths[PULSE_DATA_SIZE] = {2, 5, 6, 4};
			
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
				SwitchHornDebounce = 255;		//indicate switch pressed
				SwitchHornStatus = 1;			//sets that a switch has been pressed
				TurnBellOff();                  // turn off the bell
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

			BellCntOn = (1 + BellCntOn) % 23;

			if(BellCntOn < bell_t_on)
			{
				SwitchBellDingStatus = 1;// turn on the horn
				LED_Red(1);
			}
			else
			{
				SwitchBellDingStatus = 0;// turn off the horn
				LED_Red(0);
			}
			
			bell_t_on = expLookupPWM[(SwitchBellCnt + 10000) >> 9];
			//bell_t_on = (bell_t_on > 10) ? 0 : bell_t_on;
			
/*
			if((SwitchBellCnt >> 9) > 13)
			{
				LED_Red(1);
			}
			else
			{
				LED_Red(0);
			}*/


			// compute the max of two things, to simulate exponential decrease
			//bell_t_on = ((SwitchBellCnt >> 12) + 3 > (SwitchBellCnt >> 13) + 6) ? (SwitchBellCnt >> 12) + 3 : (SwitchBellCnt >> 13) + 6;			

			//bell_flip_cnt = 0;
//			if (SwitchBellCnt % 1000 == 0){
	//			if (bell_t_on > 0)
		//			{
						//bell_t_on --;
			//		}
			//}
			


		
/*
		uint64_t currentCenter = pulseCenters[BellCntOn];
		//uint32_t currentWidth = pulseWidths[BellCntOn];
		uint64_t precomputedValue = (SWITCH_BELL_DELAY - SwitchBellCnt) % 40;//40;

		if(precomputedValue == currentCenter)
		{
			SwitchBellDingStatus = 1;// turn on the horn
			LED_Red(1);
		}
		else if(precomputedValue == (currentCenter + pulseWidths[BellCntOn]))
		{
			SwitchBellDingStatus = 0;// turn off the horn
			// increment to the next pulse 
			BellCntOn = (1 + BellCntOn) % PULSE_DATA_SIZE;
			LED_Red(0);
		}
*/

//		if ((SwitchBellCnt % 1000) == 0)
/*
		if (1 == 2)
		{
			for(int i = 0; i < PULSE_DATA_SIZE; i++)
			{
				if (pulseWidths[i] > 1)
				{
					pulseWidths[i] = pulseWidths[i] - 1;
				}
			}
		}*/
			//pulseWidths[PULSE_DATA_SIZE] = {2, 5, 6, 4};

		// loops, 0,1,2,3

//		if (precomputedValue == (currentCenter + pulseWidths[BellCntOn])) {LED_Red(1); RTC_delayMS(1);} else LED_Red(0);
	//	if (precomputedValue == 1) {LED_Red(1); RTC_delayMS(1);} else LED_Red(0);
	//if ((currentCenter + pulseWidths[BellCntOn]) == 1) {LED_Red(1); RTC_delayMS(1);} else LED_Red(0);
	//if (currentCenter == 1) {LED_Red(1);} else LED_Red(0);



//regular turn entire bell process on or off
		if(SwitchBellCnt > 0)
		{
			SwitchBellCnt--;
		} else
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
	return SwitchBellDingStatus;
}



