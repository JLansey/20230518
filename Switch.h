/*****************************************************************************************
**
**  Switch.c
**
**  Switch Functions
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

//timing defines
#define TIME_SWITCH_PRESS_DET	30	//Time in mS to detect Switch pressed
#define TIME_SWITCH_RELEASE_DET 30	//Time in mS to detect Switch released


#define SWITCH_HORN_DEBOUNCE_INITIAL 14 // Starting the debounce counter up closer
	
//Horn Switch
#define SWITCH_HORN_PORT		PORTB
#define SWITCH_HORN_PIN			1
#define SWITCH_HORN_BIT			(1 << SWITCH_HORN_PIN)
#define SWITCH_HORN_CTRL		PORTB.PIN1CTRL

//Switch macros
//#define	SwitchPowerPressed()	(!(SwitchInputs & SWITCH_POWER))
//#define	SwitchUser1Pressed()	(!(SwitchInputs & SWITCH_USER1))
//#define SwitchUser2Pressed()	(!(SwitchInputs & SWITCH_USER2))

//Prototypes
void SwitchInit (void);
void SwitchUpdate(void);
uint8_t SwitchHornGetStatus(void);
void SwitchClearHornStatus(void);
