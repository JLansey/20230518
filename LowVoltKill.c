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
#include "Config.h"

uint16_t LowVoltDetectCount;
uint8_t LowVoltDetected;
uint16_t LowVoltkillTimer_mS;
uint16_t LowVoltkillOldTick;
uint8_t LowVoltState;

uint16_t BellDebounceTimer_mS;
uint16_t MiniHonkTimer_mS;

SpeakerState BellState;
uint8_t StartedByButton;

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
	DAC0.DATA = LOW_VOLT_KILL_DAC_CNT;
   
	//Setup AC
	AC0.MUXCTRLA = AC_MUXPOS_PIN0_gc | AC_MUXNEG_DAC_gc | (0 << AC_INVERT_bp);
	AC0.CTRLA = AC_RUNSTDBY_bm| AC_ENABLE_bm | AC_INTMODE_POSEDGE_gc;
	
	LowVoltState = LOW_VOLT_STATE_INIT;
	MiniHonkTimer_mS = 0;
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

		// see if you have been holding the button down long enough to honk or not
		if(BellDebounceTimer_mS)
		{
			BellDebounceTimer_mS--;
			LED_Red(1);
		}
		else{
			LED_Red(0);
		}
		
		// Mini honk extension timer
		if(MiniHonkTimer_mS)
		{
			MiniHonkTimer_mS--;
			if(MiniHonkTimer_mS == 0)
			{
				Horn_Enable(HORN_OFF);
			}
		}

		switch (LowVoltState)
		{
			case LOW_VOLT_STATE_INIT:
			{
				DAC0.DATA = LOW_VOLT_KILL_DAC_CNT;
				LowVoltkillTimer_mS = LOW_VOLT_KILL_TIMEOUT; // 10
				BellDebounceTimer_mS = BELL_DEBOUNCE_T;
				BellState = BELL;
				MiniHonkTimer_mS = 0;

				LowVoltDetectCount = 0;
				LowVoltDetected = 0;

				LowVoltState = LOW_VOLT_STATE_KILL;

				if (SwitchHornGetStatus())
				{
					BellState = BELL;
				}
				else
				{
					BellState = BELL_CHARGING;
				}

				break;
			}

			//if battery already low without honking horn.  Just shut down with Red LED on
			case LOW_VOLT_STATE_KILL:
			{
				if(!(AC0.STATUS & AC_STATE_bm))
				{
					LowVoltState = LOW_VOLT_STATE_DEAD;
				}

				if(LowVoltkillTimer_mS == 0)
				{
					
					DAC0.DATA = LOW_VOLT_LOW_BATT_DAC_CNT;
					LowVoltkillTimer_mS = LOW_VOLT_TIME_MAX_HORN_ON_TIME;
					if(SwitchHornGetStatus())
					{
						LowVoltState = LOW_VOLT_STATE_CHECK_HORN1;
					}
					else
					{ // pretend like horn was pressed no matter what 
						LowVoltState = LOW_VOLT_STATE_CHECK_HORN1;
					}
				}
				
				break;
			}
			
			
			//Ring Bell for end of cycle or do mini honk extension
			case LOW_VOLT_STATE_CHECK_BELL:
			{
				#if CONFIG_MODE == CONFIG_MODE_MINIBELL
				// Original bell behavior
				if(Bell_Update(BellState))
				{
					if(SwitchHornGetStatus())
					{
						LowVoltkillTimer_mS = LOW_VOLT_TIME_MAX_HORN_ON_TIME;
						BellDebounceTimer_mS = BELL_DEBOUNCE_T;
						BellState = BELL;
						LED_Green(1);

						LowVoltState = LOW_VOLT_STATE_CHECK_HORN1;
					}
				
					else if(LowVoltkillTimer_mS == 0 && LowVoltDetected)
					{
						Bell_Init();
						LED_Green(0);

						LowVoltkillTimer_mS = LOW_VOLT_LOW_BATT_BEEP;
						LowVoltState = LOW_VOLT_STATE_END_BEEP;
					}
				}
				else
				{
					LowVoltkillTimer_mS = LOW_VOLT_TIME_WAIT_LOW_BATT_BEEP;
					LowVoltState = LOW_VOLT_STATE_CHECK_HORN0;
				}
				#else
				// Mini mode - short honk extension instead of bell
				if(MiniHonkTimer_mS == 0)
				{
					// Do a short honk extension
					Horn_Enable(HORN_ON);
					MiniHonkTimer_mS = MINI_HONK_EXTENSION_TIME;
					
					if(SwitchHornGetStatus())
					{
						LowVoltkillTimer_mS = LOW_VOLT_TIME_MAX_HORN_ON_TIME;
						BellDebounceTimer_mS = BELL_DEBOUNCE_T;
						LED_Green(1);

						LowVoltState = LOW_VOLT_STATE_CHECK_HORN1;
					}
					else if(LowVoltkillTimer_mS == 0 && LowVoltDetected)
					{
						LED_Green(0);
						LowVoltkillTimer_mS = LOW_VOLT_LOW_BATT_BEEP;
						
						// For both modes, we properly initialize the low battery bell
						Horn_Enable(BELL_LOWVOLT);
						
						LowVoltState = LOW_VOLT_STATE_END_BEEP;
					}
					else
					{
						LowVoltkillTimer_mS = LOW_VOLT_TIME_WAIT_LOW_BATT_BEEP;
						LowVoltState = LOW_VOLT_STATE_CHECK_HORN0;
					}
				}
				#endif

				break;
			}

			//Horn switch detected not pressed.  wait here until horn pressed again, or power dies
			case LOW_VOLT_STATE_CHECK_HORN0:
			{
				// Only turn off horn if mini honk timer is not active
				if(MiniHonkTimer_mS == 0)
				{
					Horn_Enable(HORN_OFF);
				}

				if(SwitchHornGetStatus())
				{
					LED_Green(1);
					LowVoltkillTimer_mS = LOW_VOLT_TIME_MAX_HORN_ON_TIME;
					BellDebounceTimer_mS = BELL_DEBOUNCE_T;
					BellState = BELL;
					// go on to the next state where it will maybe honk
					LowVoltState = LOW_VOLT_STATE_CHECK_HORN1;
				}
				else if(LowVoltkillTimer_mS == 0 && LowVoltDetected)
				{
					LED_Green(0);
					LowVoltkillTimer_mS = LOW_VOLT_LOW_BATT_BEEP;
					
					// For both modes, we properly initialize the low battery bell
					Horn_Enable(BELL_LOWVOLT);
					
					LowVoltState = LOW_VOLT_STATE_END_BEEP;
				}

				break;
			}

			//Horn Switch detected pressed.  Honk Horn and check battery level.  If detected low for period of time go to LOW_VOLT_STATE_BATT_LOW state
			case LOW_VOLT_STATE_CHECK_HORN1:
			{
				//check the switch is still pressed
				if(SwitchHornGetStatus())
				{
					// if you are commited to honk
					if (BellDebounceTimer_mS == 0){
						Horn_Enable(HORN_ON);
						LED_Green(1);
					}

					//stop honking horn if max on time expired
					if(LowVoltkillTimer_mS == 0)
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
					LED_Green(0);

					LowVoltkillTimer_mS = LOW_VOLT_TIME_WAIT_LOW_BATT_BEEP;
					LowVoltState = LOW_VOLT_STATE_CHECK_BELL;
					
					#if CONFIG_MODE == CONFIG_MODE_MINIBELL
					BellState = BELL;
					Horn_Enable(BellState);
					#endif
				}
				break;
			}

			//Beep horn to indicate battery low.  Go back to LOW_VOLT_STATE_BATT_LOW state if horn switch pressed again
			case LOW_VOLT_STATE_END_BEEP:
			{
				LED_Green(1);

				if(SwitchHornGetStatus())
				{
					LowVoltkillTimer_mS = LOW_VOLT_TIME_MAX_HORN_ON_TIME;
					BellDebounceTimer_mS = BELL_DEBOUNCE_T;

					BellState = BELL;
					LED_Green(1);
					
					LowVoltState = LOW_VOLT_STATE_CHECK_HORN1;
				}
				else
				{
					LED_Green(0);

					// Both config modes use the same Bell_Update for low battery
					if(Bell_Update(BELL_LOWVOLT) == 0)
					{
						// If the Bell_Update returns 0, the sequence is done
						// Make sure to turn the horn off completely
						Horn_Enable(HORN_OFF);
						
						// Reset timer to prevent any further action
						if(LowVoltkillTimer_mS != 0)
						{
							LowVoltkillTimer_mS = 0;
						}
					}
				}
				break;
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