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
#include "Timer.h"
#include "Led.h"

//Variables Global to the LED functions
uint16_t Status_Led_oldtick;				//used for status LED timing
uint16_t Status_Led_Timer;					//used for status flash rate


//*--------------------------------------------------------------------------------------
//* Function Name       : LED_init()
//* Object              : initialize LED hardware
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void LED_init(void)
{
   //Status_Led_Timer = TIME_LED_STATUS_HEARTBEAT;
   LED_RED_PORT.OUTCLR = LED_RED_BIT;
   LED_GREEN_PORT.OUTCLR = LED_GREEN_BIT;

   LED_RED_PORT.DIRSET = LED_RED_BIT;
   LED_GREEN_PORT.DIRSET = LED_GREEN_BIT;

   Status_Led_Timer = TIME_LED_STATUS_HEARTBEAT;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : LED_Red()
//* Object              : enable or disable Red LED
//* Input Parameters    : uint8_t Enable = true if light RED LED
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void LED_Red(uint8_t Enable)
{
	if(Enable)
	{
		LED_RED_PORT.OUTSET = LED_RED_BIT;
	}
	else
	{
		LED_RED_PORT.OUTCLR = LED_RED_BIT;
	}
}


//*--------------------------------------------------------------------------------------
//* Function Name       : LED_Green()
//* Object              : enable or disable Green LED
//* Input Parameters    : uint8_t Enable = true if light Green LED
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void LED_Green(uint8_t Enable)
{
	if(Enable)
	{
		LED_GREEN_PORT.OUTSET = LED_GREEN_BIT;
	}
	else
	{
		LED_GREEN_PORT.OUTCLR = LED_GREEN_BIT;
	}
}



//*--------------------------------------------------------------------------------------
//* Function Name       : LED_update()
//* Object              : Control the Status LED based on input Status I/O pin.
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void LED_update()
{
	if(RTC_getTick() != Status_Led_oldtick)
	{
		Status_Led_oldtick = RTC_getTick();

		if(Status_Led_Timer)
		{
			Status_Led_Timer--;
		}

		else
		{
			LED_RED_PORT.OUTTGL = LED_RED_BIT;
			Status_Led_Timer = TIME_LED_STATUS_HEARTBEAT;
		}
	}        
}   
