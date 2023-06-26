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
#include <util/delay.h>


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
//* Function Name       : Horn_init()
//* Object              : setup I/O pins used by horn
//* Input Parameters    : uint8_t Enable = true to turn horn on
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void safe_delay(float t)
{
	
	for(int i = 0; i < t * 1000; i++) {
		_delay_ms(.0001);
	}
	
}

void Horn_Bell()
{
	float on_time;
	float cur_on_time;
	// total time 0.037
	on_time = 0.025;

	for(int cnt = 0; cnt < 1000; cnt++) {
				
		// total time 0.037
		cur_on_time = on_time - on_time * cnt / 1000;
				
		HORN_PORT.OUTSET = HORN_BIT;
		safe_delay(cur_on_time);

		HORN_PORT.OUTCLR = HORN_BIT;
		safe_delay(0.027 - cur_on_time);
				
		//0.0135
				
	}


}


void Horn_Enable(uint8_t Enable)
{
	float on_time;
	// total time 0.037
	on_time = 0.0235;

	if(Enable)
	{		
			HORN_PORT.OUTSET = HORN_BIT;
			safe_delay(on_time);

			HORN_PORT.OUTCLR = HORN_BIT;
			safe_delay(0.037 - 0.0235);
			
			//0.0135
	
		//for(int i = 0; i < 1; i++) {
			//_delay_ms(.1);
		//}
		//
		//HORN_PORT.OUTCLR = HORN_BIT;
		//
		//for(int i = 0; i < 5; i++) {
			//_delay_ms(.1);
		//}
	}
	else
	{
		HORN_PORT.OUTCLR = HORN_BIT;
	}
}

