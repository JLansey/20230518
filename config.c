/*****************************************************************************************
**
**  Config.c
**
**  Configuration Functions for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

#include <avr/io.h>
#include <avr/eeprom.h>
#include "Config.h"
#include "Timer.h"
#include "Led.h"
#include "Switch.h"
#include "Charger.h"

// Current configuration mode
uint8_t current_config_mode;

// Button press tracking variables
uint8_t config_press_count;
uint16_t config_press_timer;
uint8_t config_button_pressed;
uint16_t config_old_tick;
uint8_t config_opportunity_used;  // Flag to track if opportunity to change config has been used

// Configuration state
typedef enum {
    CONFIG_STATE_IDLE,
    CONFIG_STATE_COUNTING,
    CONFIG_STATE_CHANGED,
    CONFIG_STATE_FEEDBACK,
    CONFIG_STATE_LOCKED    // New state: config is locked until device is unplugged
} ConfigState;

ConfigState config_state;

//*--------------------------------------------------------------------------------------
//* Function Name       : Config_init()
//* Object              : Initialize configuration
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void Config_init(void)
{
    // Read configuration from EEPROM
    uint8_t stored_mode = eeprom_read_byte((uint8_t*)CONFIG_EEPROM_ADDRESS);
    
    // Check if the stored mode is valid (currently only 0 or 1)
    if (stored_mode <= CONFIG_MODE_MINI) {
        current_config_mode = stored_mode;
    } else {
        // If not valid, use default and write it to EEPROM
        current_config_mode = CONFIG_MODE_DEFAULT;
        eeprom_write_byte((uint8_t*)CONFIG_EEPROM_ADDRESS, current_config_mode);
    }
    
    // Initialize button tracking
    config_press_count = 0;
    config_press_timer = 0;
    config_button_pressed = 0;
    config_opportunity_used = 0;  // Start with fresh opportunity
    config_state = CONFIG_STATE_IDLE;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : Config_get_mode()
//* Object              : Get current configuration mode
//* Input Parameters    : none
//* Output Parameters   : uint8_t current mode
//*--------------------------------------------------------------------------------------

uint8_t Config_get_mode(void)
{
    return current_config_mode;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : Config_update()
//* Object              : Process configuration changes
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void Config_update(void)
{
    // Make sure we only process when charging
    if (CHARGER_PWR_GOOD_PORT.IN & CHARGER_PWR_GOOD_BIT) {
        // Not charging, reset everything for next time
        config_press_count = 0;
        config_state = CONFIG_STATE_IDLE;
        config_opportunity_used = 0; // Reset the opportunity when unplugged
        return;
    }
    
    // Check if time to process
    if (RTC_getTick() != config_old_tick) {
        config_old_tick = RTC_getTick();
        
        // Check if timer active
        if (config_press_timer > 0) {
            config_press_timer--;
            
            // Timer expired, check what happened
            if (config_press_timer == 0 && config_state == CONFIG_STATE_COUNTING) {
                // Check if exactly 5 presses and first opportunity
                if (config_press_count == CONFIG_CHANGE_REQUIRED_PRESSES && !config_opportunity_used) {
                    // Toggle configuration
                    current_config_mode = (current_config_mode == CONFIG_MODE_MINIBELL) ? 
                                           CONFIG_MODE_MINI : CONFIG_MODE_MINIBELL;
                    
                    // Save to EEPROM
                    eeprom_write_byte((uint8_t*)CONFIG_EEPROM_ADDRESS, current_config_mode);
                    
                    // Mark opportunity as used
                    config_opportunity_used = 1;
                    
                    // Change to feedback state
                    config_state = CONFIG_STATE_CHANGED;
                    config_press_timer = 1000; // Give feedback for 1 second
                } else {
                    // Either wrong count or opportunity already used
                    if (!config_opportunity_used) {
                        // Wrong count but still have opportunity
                        config_press_count = 0;
                        config_state = CONFIG_STATE_IDLE;
                    } else {
                        // Opportunity already used
                        config_state = CONFIG_STATE_LOCKED;
                    }
                }
            }
        }
        
        // Visual feedback states
        if (config_state == CONFIG_STATE_CHANGED) {
            // Flash both LEDs to indicate configuration changed
            LED_Red(1);
            LED_Green(1);
            
            if (config_press_timer == 0) {
                config_state = CONFIG_STATE_FEEDBACK;
                config_press_timer = 2000; // 2 seconds of mode feedback
            }
        } else if (config_state == CONFIG_STATE_FEEDBACK) {
            // Show which mode is active
            if (current_config_mode == CONFIG_MODE_MINIBELL) {
                // Alternate RED/GREEN for MINIBELL mode
                LED_Red(RTC_getTick() & 0x100);
                LED_Green(!(RTC_getTick() & 0x100));
            } else {
                // Flash both LEDs together for MINI mode
                LED_Red(RTC_getTick() & 0x100);
                LED_Green(RTC_getTick() & 0x100);
            }
            
            if (config_press_timer == 0) {
                // Configuration change is done, lock it
                config_state = CONFIG_STATE_LOCKED;
            }
        } else if (config_state == CONFIG_STATE_LOCKED) {
            // No further configuration changes allowed until unplugged
            // Just handle the button press without counting
            uint8_t button_status = SwitchHornGetStatus();
            
            if (button_status && !config_button_pressed) {
                // Button pressed - just track state but don't count
                config_button_pressed = 1;
            } else if (!button_status && config_button_pressed) {
                // Button released
                config_button_pressed = 0;
            }
            
            return;
        }
        
        // Process button input for configuration (only if not locked)
        if (config_state != CONFIG_STATE_LOCKED) {
            uint8_t button_status = SwitchHornGetStatus();
            
            if (button_status && !config_button_pressed) {
                // Button newly pressed
                config_button_pressed = 1;
                
                // Only count if we're in a counting state or idle
                if (config_state == CONFIG_STATE_IDLE || config_state == CONFIG_STATE_COUNTING) {
                    config_press_count++;
                    config_press_timer = CONFIG_CHANGE_TIMEOUT;
                    config_state = CONFIG_STATE_COUNTING;
                    
                    // If more than 5 presses, opportunity is used up
                    if (config_press_count > CONFIG_CHANGE_REQUIRED_PRESSES) {
                        config_opportunity_used = 1;
                        config_state = CONFIG_STATE_LOCKED;
                    }
                }
            } else if (!button_status && config_button_pressed) {
                // Button released
                config_button_pressed = 0;
            }
        }
    }
}