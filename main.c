/*****************************************************************************************
**
**  main.c
**
**  Main Functions for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <assert.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "Switch.h"
//#include "ADC.h"
#include "Timer.h"
#include "Led.h"
#include "Charger.h"
#include "Horn.h"
#include "LowVoltKill.h"

#define BOOTEND_FUSE               (0x00)

FUSES = {
	.WDTCFG = PERIOD_2KCLK_gc | WINDOW_OFF_gc, // brownout detect voltage
	.BODCFG = ACTIVE_ENABLED_gc | LVL_BODLEVEL7_gc,
	//.OSCCFG = FREQSEL_20MHZ_gc,
	.OSCCFG = FREQSEL_16MHZ_gc,
	.reserved_1 = {0xFF},
	.TCD0CFG = 0x00,
	.SYSCFG0 = CRCSRC_NOCRC_gc | RSTPINCFG_UPDI_gc,
	.SYSCFG1 = SUT_64MS_gc,
//	.SYSCFG1 = SUT_4MS_gc, // startup time for the processer, how long do you wait
	.APPEND = 0x00,
	.BOOTEND = BOOTEND_FUSE
};

uint8_t LowSpeed;


int main(void)
{
	/* Fix the clock */
 	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PEN_bm | (0 << CLKCTRL_PDIV0_bp));
 	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
	
	RTC_init();
	LED_init();
	SwitchInit();
	Charger_init();
	// Bell_Init();
	LowVoltKill_init();
	
	LowSpeed = 0;
	
	// Enable interrupts
	sei();
	
	while (1)
	{
		wdt_reset();
		SwitchUpdate();

		//If Charging: LED's are controlled by Charger, and horn is forced off
		if(!(CHARGER_PWR_GOOD_PORT.IN & CHARGER_PWR_GOOD_BIT))
		{
			if(LowSpeed == 0)
			{
				_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PEN_bm | (0 << CLKCTRL_PDIV0_bp));
				_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCULP32K_gc);
				LowSpeed = 1;
			}

			Horn_Enable(HORN_OFF);

			//LED_update();
			if(!(CHARGER_STATUS_PORT.IN & CHARGER_STATUS_BIT))
			{
				LED_Red(1);
			}
			else
			{
				LED_Red(0);
			}
		}

		else
		//Not charging, honk horn unless fault found
		{
			if(LowSpeed == 1)
			{
 				_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PEN_bm | (0 << CLKCTRL_PDIV0_bp));
 				_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
				LowSpeed = 0;
			}

			//LED_update();
			//LED_Green(0);
			LowVoltKill_update();
		}
	}
}
