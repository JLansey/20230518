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
#include <avr/eeprom.h>

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

// EEPROM configuration
#define CONFIG_EEPROM_ADDR         0x00
#define CONFIG_EEPROM_SIG          0xAA

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

// Global config_mode variable (this will be used by code added to LowVoltKill.c)
uint8_t eeprom_config_mode = CONFIG_MODE_MINIBELL;

// Variables for button hold feature
uint16_t buttonHoldStartTime = 0;      // When we started holding the button
uint8_t buttonWasReleased = 1;         // Has the button been released since last hold

// Read the configuration from EEPROM
static void read_config_from_eeprom(void)
{
	uint8_t signature = eeprom_read_byte((uint8_t*)CONFIG_EEPROM_ADDR);
	uint8_t stored_mode = eeprom_read_byte((uint8_t*)(CONFIG_EEPROM_ADDR + 1));
	
	// If valid signature and mode, use the stored value
	if (signature == CONFIG_EEPROM_SIG &&
	(stored_mode == CONFIG_MODE_MINIBELL || stored_mode == CONFIG_MODE_MINI)) {
		eeprom_config_mode = stored_mode;
		} else {
		// Otherwise use the default mode and save it
		eeprom_config_mode = CONFIG_MODE_MINIBELL;
		// Write signature byte
		eeprom_write_byte((uint8_t*)CONFIG_EEPROM_ADDR, CONFIG_EEPROM_SIG);
		// Write configuration mode
		eeprom_write_byte((uint8_t*)(CONFIG_EEPROM_ADDR + 1), eeprom_config_mode);
	}
}

// Toggle between configuration modes and provide visual feedback
static void toggle_config_mode(void)
{
	// Toggle the mode
	eeprom_config_mode = (eeprom_config_mode == CONFIG_MODE_MINIBELL) ?
	CONFIG_MODE_MINI : CONFIG_MODE_MINIBELL;
	
	// Save the new mode to EEPROM
	eeprom_write_byte((uint8_t*)CONFIG_EEPROM_ADDR, CONFIG_EEPROM_SIG);
	eeprom_write_byte((uint8_t*)(CONFIG_EEPROM_ADDR + 1), eeprom_config_mode);
	
	// Indicate the current mode through LED flashes
	if (eeprom_config_mode == CONFIG_MODE_MINIBELL) {
		// Flash green LED to indicate MiniBell mode
		for (int i = 0; i < 3; i++) {
			LED_Green(1);
			RTC_delayMS(200);
			LED_Green(0);
			RTC_delayMS(200);
		}
		} else {
		// Flash red LED to indicate Mini mode
		for (int i = 0; i < 3; i++) {
			LED_Red(1);
			RTC_delayMS(200);
			LED_Red(0);
			RTC_delayMS(200);
		}
	}
}

int main(void)
{
	/* Fix the clock */
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PEN_bm | (0 << CLKCTRL_PDIV0_bp));
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
	
	RTC_init();
	LED_init();
	SwitchInit();
	Charger_init();
	
	// Read configuration from EEPROM
	read_config_from_eeprom();
	
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
				
				// If held for threshold time, toggle configuration
				if (holdTime >= BUTTON_HOLD_THRESHOLD) {
					// Toggle configuration mode
					toggle_config_mode();
					
					// After toggling, reset hold time to prevent repeated toggling
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