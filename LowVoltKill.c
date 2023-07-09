/*****************************************************************************************
**
**  LowVoltKill.c
**
**  Low Voltage Kill Functions for Tiny1616
**  uses AC (analog compare) to quickly turn off horn
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "Timer.h"
#include "LowVoltKill.h"
#include "Switch.h"
#include "Horn.h"
#include "Led.h"

uint16_t LowVoltDetectCount;
uint8_t LowVoltDetected;
uint16_t LowVoltkillTimer_mS;
uint16_t LowVoltkillOldTick;
uint8_t LowVoltState;

#define LOW_VOLT_STATE_INIT			0
#define LOW_VOLT_STATE_KILL			1
#define LOW_VOLT_STATE_CHECK_HORN0	2
#define LOW_VOLT_STATE_CHECK_HORN1	3
#define LOW_VOLT_STATE_END_BEEP		4
#define LOW_VOLT_STATE_DEAD			5


//*--------------------------------------------------------------------------------------
//* Function Name       : LowVoltKill_init()
//* Object              : initialize the AC and DAC for fast shutdown
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void LowVoltKill_init(void)
{
	//configure pins
	VOLT_KILL_AC_PORT.OUTCLR = VOLT_KILL_AC_BIT;
	VOLT_KILL_ADC_PORT.OUTCLR = VOLT_KILL_ADC_BIT;

	//disable inputs
	VOLT_KILL_AC_CTRL = (4 << PORT_ISC0_bp);
	VOLT_KILL_ADC_CTRL = (4 << PORT_ISC0_bp);
   
	//Setup DAC
	DAC0.CTRLA = DAC_OUTEN_bm | DAC_ENABLE_bm;
	VREF.CTRLA = VREF_DAC0REFSEL_1V1_gc | VREF_ADC0REFSEL_1V1_gc;
	DAC0.DATA = LOW_VOLT_KILL_DAC_CNT; // dac on one side, battery on the other
   
	//Setup AC
	AC0.MUXCTRLA = AC_MUXPOS_PIN0_gc | AC_MUXNEG_DAC_gc | (0 << AC_INVERT_bp);
	AC0.CTRLA = AC_RUNSTDBY_bm| AC_ENABLE_bm | AC_INTMODE_POSEDGE_gc;
	
	LowVoltState = LOW_VOLT_STATE_INIT;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : LowVoltKill_update
//* Object              : Update the Low Voltage kill function
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void LowVoltKill_update(void)
{
	if(RTC_getTick() != LowVoltkillOldTick)
	{
		LowVoltkillOldTick = RTC_getTick();

		if(LowVoltkillTimer_mS)
		{
			LowVoltkillTimer_mS--;
		}

		switch (LowVoltState)
		{
			case LOW_VOLT_STATE_INIT:
			{
				DAC0.DATA = LOW_VOLT_KILL_DAC_CNT;
				LowVoltkillTimer_mS = LOW_VOLT_KILL_TIMEOUT;
				LowVoltDetectCount = 0;
				LowVoltDetected = 0;

				LowVoltState = LOW_VOLT_STATE_KILL;
				break;
			}

			//if battery already low without honking horn.  Just shut down with Red LED on
			case LOW_VOLT_STATE_KILL:
			{
				if(!(AC0.STATUS & AC_STATE_bm)) // AC0.STATUS - the analog comparitor to compare voltages
				{
					LowVoltState = LOW_VOLT_STATE_DEAD;
				}

				if(LowVoltkillTimer_mS == 0)
				{
					DAC0.DATA = LOW_VOLT_LOW_BATT_DAC_CNT;
					LowVoltkillTimer_mS = LOW_VOLT_TIME_MAX_HORN_ON_TIME;
					LowVoltState = LOW_VOLT_STATE_CHECK_HORN1;
				}
				break;
			}

			//Horn switch detected not pressed.  wait here until horn pressed again, or power dies
			case LOW_VOLT_STATE_CHECK_HORN0:
			{
				Horn_Enable(0);
				//LED_Green(0);

				if(SwitchHornGetStatus())
				{
					LowVoltkillTimer_mS = LOW_VOLT_TIME_MAX_HORN_ON_TIME;
					LowVoltState = LOW_VOLT_STATE_CHECK_HORN1;
				}
				
				else if(LowVoltkillTimer_mS == 0 && LowVoltDetected)
				{
					// turn back on - is necessary for low volt kill
					Horn_Enable(1);
					//LED_Green(1);
					//Horn_Enable(0);
					LowVoltkillTimer_mS = LOW_VOLT_LOW_BATT_BEEP;
					LowVoltState = LOW_VOLT_STATE_END_BEEP;
				}

				break;
			}

			//Horn Switch detected pressed.  Honk Horn and check battery level.  If detected low for period of time go to LOW_VOLT_STATE_BATT_LOW state
			case LOW_VOLT_STATE_CHECK_HORN1:
			{
				if(SwitchHornGetStatus())
				{
					// TURN the horn back on here if you want to!!!
					Horn_Enable(1);
					//Horn_Enable(0);
					//LED_Green(1);

					//stop honking horn if max on time expired
					if(LowVoltkillTimer_mS  == 0)
					{
						SwitchClearHornStatus();
					}

					//check Low battery condition over time
					if(!(AC0.STATUS & AC_STATE_bm))
					{
						LowVoltDetectCount++;
						if(LowVoltDetectCount >= LOW_VOLT_LOW_BATT_DET_TIME)
						{
							LowVoltDetected = 1;
							LED_Red(1);
						}
					}
					else
					{
						if(LowVoltDetectCount)
						{
							LowVoltDetectCount--;
						}
					}
				}
				else
				{
					LowVoltkillTimer_mS = LOW_VOLT_TIME_WAIT_LOW_BATT_BEEP;
					LowVoltState = LOW_VOLT_STATE_CHECK_HORN0;
				}
				break;
			}

			//Beep horn to indicate battery low.  Go back to LOW_VOLT_STATE_BATT_LOW state if horn switch pressed again
			case LOW_VOLT_STATE_END_BEEP:
			{
				if(SwitchHornGetStatus())
				{
					LowVoltkillTimer_mS = LOW_VOLT_TIME_MAX_HORN_ON_TIME;
					LowVoltState = LOW_VOLT_STATE_CHECK_HORN1;
				}
			
				if(LowVoltkillTimer_mS == 0)
				{
					Horn_Enable(0);
					//LED_Green(0);
				}
			}

			//Just stay here with RED LED on.  Battery is dead
			case LOW_VOLT_STATE_DEAD:
			{
				LED_Red(1);
				break;
			}
		}
	}
}


void BellUpdateRing()
{
	if (SwitchBellGetStatus())
	{
		// switch from on to off if we need to
		if (GetBellSpeakerStatus())
		{
			Horn_Enable(1);
		}
		else
		{
			Horn_Enable(0);
		}
	}

}