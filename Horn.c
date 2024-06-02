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
#include "Timer.h"

// Create a table of PWM settings
PWMSetting pwm_settings[]=
{
	{1, 2500, 50},
	{400, 1000, 50},
	{400, 1500, 50},
	{400, 2000, 50},
	{400, 2500, 50},
	{0, 2500, 100},
};

uint16_t Horn_Timer;
uint8_t Horn_Index;
uint16_t Horn_OldTick;


//*--------------------------------------------------------------------------------------
//* Function Name       : Bell_Init()
//* Object              : setup I/O pins used by horn
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void Bell_Init(void)
{
	// Configure TCA0 for single-slope PWM
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm;

	// Set TOP value for 1 kHz PWM
	TCA0.SINGLE.PER = (F_CPU / 64 / 1000) - 1; // Assuming F_CPU = 20 MHz and prescaler of 64

	// Set duty cycle
	TCA0.SINGLE.CMP0 = 0;

	// Set the prescaler and enable TCA0
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV64_gc | TCA_SINGLE_ENABLE_bm;
	
	// Set HORN as an output
	HORN_PORT.DIRSET = HORN_BIT;

	Horn_Timer = 0;
	Horn_Index = 0;

	Horn_Timer = pwm_settings[Horn_Index].TimeNextStep;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : Horn_Update()
//* Object              : update the horn engine
//* Input Parameters    : none
//* Output Parameters   : uint8_t = 1 if Bell still running
//*--------------------------------------------------------------------------------------

uint8_t Horn_Update(void)
{
	uint8_t Status = 0;

	if (Horn_OldTick != RTC_getTick())
	{
		Horn_OldTick = RTC_getTick();

		if (Horn_Timer)
		{
			Status = 1;
			Horn_Timer--;
		}
		else
		{
			if (pwm_settings[Horn_Index].TimeNextStep != 0)
			{
				Status = 1;

				Horn_Index++; // Advance to the next cycle

				// Clamp the frequency to a valid range
				uint16_t frequency = pwm_settings[Horn_Index].frequency;
				if (frequency < MIN_FREQ)
				frequency = MIN_FREQ;
				else if (frequency > MAX_FREQ)
				frequency = MAX_FREQ;

				// Calculate the top value based on frequency
				uint16_t top_value = (HORN_CPU_CLOCK / 64 / frequency) - 1; // Prescaler of 64

				// Ensure the top value is within a valid range
				if (top_value == 0 || top_value >= 65536)
				top_value = 65535;

				// Set the double-buffered PER value for the desired frequency
				TCA0.SINGLE.PERBUF = top_value;

				// Limit duty cycle to 100%
				uint8_t duty_cycle = pwm_settings[Horn_Index].duty_cycle;
				if (duty_cycle > 100)
				duty_cycle = 100;

				// Calculate and set the double-buffered CMP0 safely
				uint16_t cmp_value = (top_value * duty_cycle) / 100;

				// Handle special case for 100% duty cycle
				if (duty_cycle == 100)
				cmp_value = top_value;

				TCA0.SINGLE.CMP0BUF = (cmp_value <= top_value) ? cmp_value : top_value;

				// Set Timer
				Horn_Timer = pwm_settings[Horn_Index].TimeNextStep;
			}
		}
	}
	return Status;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : Horn_Enable()
//* Object              : Horn enable Function, setup the GPIO based on input
//* Input Parameters    : uint8_t Enable = true to turn horn on
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void Horn_Enable(uint8_t Enable)
{
	if(Enable == HORN_OFF)
	{
		// Disable PWM
		TCA0.SINGLE.CTRLA = 0;
		TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP0EN_bm;

		HORN_PORT.DIRSET = HORN_BIT;
		HORN_PORT.OUTCLR = HORN_BIT;
	}

	else if(Enable == HORN_ON)
	{
		// Disable PWM
		TCA0.SINGLE.CTRLA = 0;
		TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP0EN_bm;

		HORN_PORT.DIRSET = HORN_BIT;
		HORN_PORT.OUTSET = HORN_BIT;
	}

	else if(Enable == HORN_BELL)
	{
		Bell_Init();
	}
}
