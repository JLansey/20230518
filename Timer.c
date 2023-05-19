#include <avr/wdt.h>/*****************************************************************************************
**
**  Timer.c
**
**  Timer Functions for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "Timer.h"

//*--------------------------------------------------------------------------------------
//* Function Name       : RTC_init()
//* Object              : Set up Timer to be used for various routine timing functions
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void RTC_init(void)
{
	while (RTC.STATUS > 0)
	{
		;										/* Wait for all register to be synchronized */
	}
	RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
	RTC.CTRLA = RTC_RTCEN_bm | RTC_RUNSTDBY_bm;	/* 1kHz Internal Crystal Oscillator (Internal 1kHz OSC) */
}  


//*--------------------------------------------------------------------------------------
//* Function Name       : RTC_getTick()
//* Object              : Return The current system timer Tick value
//* Input Parameters    : none
//* Output Parameters   : Timer 0 Tick counter
//*--------------------------------------------------------------------------------------

inline uint16_t RTC_getTick(void)
{
	return RTC.CNT;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : RTC_delayMS()
//* Object              : Delay number of mS passed in
//* Input Parameters    : unsigned int DelayValue = delay to wait x 1mS
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void RTC_delayMS(uint16_t DelayValue)
{
   uint16_t oldtick;

   oldtick = RTC_getTick();

   while(DelayValue != 0)
   {
      if(RTC_getTick() != oldtick)
      {
         oldtick = RTC_getTick();
         DelayValue--;
         wdt_reset();
      }
   }
}
