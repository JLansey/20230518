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
#include "Config.h"

#define BOOTEND_FUSE               (0x00)

// Number of milliseconds to hold the button during charging
#define BUTTON_HOLD_THRESHOLD      5000  // 5 seconds

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

// Variables for button hold feature
uint16_t buttonHoldStartTime = 0;      // When we started holding the button
uint8_t buttonWasReleased = 1;         // Has the button been released since last hold

int main(void)
{
	/* Fix the clock */
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PEN_bm | (0 << CLKCTRL_PDIV0_bp));
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
	
	RTC_init();
	LED_init();
	SwitchInit();
	Charger_init();
	// Bell_Init(); // This is now handled in LowVoltKill_init() as needed
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

			// Get the current tick
			uint16_t currentTick = RTC_getTick();

			// Check if button is pressed while charging
			if(SwitchHornGetStatus())
			{
				// Button is pressed
				LED_Red(0);
				LED_Green(0);
				
				// If this is a new button press, record the start time
				if (buttonWasReleased) {
					buttonHoldStartTime = currentTick;
					buttonWasReleased = 0;
				}
				
				// Check if button has been held long enough
				uint16_t holdTime;
				if (currentTick >= buttonHoldStartTime) {
					holdTime = currentTick - buttonHoldStartTime;
					} else {
					// Handle 16-bit overflow
					holdTime = (65535 - buttonHoldStartTime) + currentTick + 1;
				}
				
				// If held for threshold time, show confirmation blinks
				if (holdTime >= BUTTON_HOLD_THRESHOLD) {
					// Signal activation with quick flashes
					for (int i = 0; i < 3; i++) {
						LED_Red(1);
						RTC_delayMS(100);
						LED_Red(0);
						RTC_delayMS(100);
					}
					
					// After flashing, reset hold time to prevent repeated flashing
					buttonHoldStartTime = currentTick;
				}
			}
			else
			{
				// Button is not pressed - note the release
				buttonWasReleased = 1;
				
				// Normal LED state for charging
				if(!(CHARGER_STATUS_PORT.IN & CHARGER_STATUS_BIT)) {
					LED_Red(1);
					LED_Green(0);
					} else {
					LED_Red(0);
					LED_Green(1);
				}
			}
		}
		else
		//Not charging, honk horn unless fault found
		{
			// Reset the button hold feature variables when not charging
			buttonWasReleased = 1;
			
			if(LowSpeed == 1)
			{
				_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PEN_bm | (0 << CLKCTRL_PDIV0_bp));
				_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
				LED_Green(0);

				LowSpeed = 0;
			}

			//LED_update();
			LowVoltKill_update();
		}
	}
}