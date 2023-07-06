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
#define TIME_SWITCH_PRESS_DET	15	//Time in mS to detect Switch pressed
#define TIME_SWITCH_RELEASE_DET 15	//Time in mS to detect Switch released
#define SWITCH_HORN_DEBOUNCE_INITIAL 14 // Starting the debounce counter up closer
		
	
	
//Horn Switch
#define SWITCH_HORN_PORT		PORTB
#define SWITCH_HORN_PIN			1
#define SWITCH_HORN_BIT			(1 << SWITCH_HORN_PIN)
#define SWITCH_HORN_CTRL		PORTB.PIN1CTRL

//Bell switch                ,
#define SWITCH_BELL_DELAY 5000UL
#define BELL_T_ON 15UL
#define BELL_T_OFF 25UL

#define PULSE_DATA_SIZE 4



//Switch macros
//#define	SwitchPowerPressed()	(!(SwitchInputs & SWITCH_POWER))
//#define	SwitchUser1Pressed()	(!(SwitchInputs & SWITCH_USER1))
//#define SwitchUser2Pressed()	(!(SwitchInputs & SWITCH_USER2))

// Function Prototypes
void SwitchInit(void);
void BellInit(void);
void TurnBellOn(void);
void TurnBellOff(void);
void SwitchUpdate(void);
uint8_t SwitchHornGetStatus(void);
void SwitchClearHornStatus(void);

void BellUpdateSwitch(void);
uint8_t SwitchBellGetStatus(void);
void SwitchClearBellStatus(void);
uint8_t GetBellSpeakerStatus(void);

