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
#include "Timer.h"

uint16_t Charge_OldTick;
uint16_t Charge_1Second_Count;
uint16_t Charge_Seconds_Count;

extern uint16_t ADC_DataBattery;

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
	
	Charger_TimerReset();
}

//*--------------------------------------------------------------------------------------
//* Function Name       : Charger_update()
//* Object              : update the charger hardware
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void Charger_update(void)
{
	if(Charge_OldTick != RTC_getTick())
	{
		Charge_OldTick = RTC_getTick();

		if(Charge_1Second_Count)
		{
			Charge_1Second_Count--;
		}
		else
		{
			if(ADC_DataBattery < CHARGE_LEVEL_CHARGED)
			{
				Charger_TimerReset();			//reset time for battery charge if ADC level is < CHARGE_LEVEL_CHARGED
			}
			Charge_1Second_Count = (CHARGE_COUNT_1_SECOND -1);

			if(Charge_Seconds_Count)
			{
				Charge_Seconds_Count--;
			}
		}
	}
}


//*--------------------------------------------------------------------------------------
//* Function Name       : Charger_TimerReset()
//* Object              : reset the max time till charge over
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void Charger_TimerReset(void)
{
	Charge_1Second_Count = 0;
	Charge_Seconds_Count = CHARGE_FULL_MAX_SECONDS;
}