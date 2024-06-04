/*****************************************************************************************
**
**  LowVoltKill.h
**
**  Low Voltage Kill Functions for Tiny1616
**  uses AC (analog compare) to quickly turn off horn
**
**  2023 CPU Ready Inc
**
******************************************************************************************/


//Timing Defines
#define LOW_VOLT_KILL_DAC_CNT				0x2D
#define LOW_VOLT_LOW_BATT_DAC_CNT			0x2f
#define LOW_VOLT_KILL_TIMEOUT				10
#define LOW_VOLT_LOW_BATT_DET_TIME			100
#define LOW_VOLT_TIME_WAIT_LOW_BATT_BEEP	1000 // time delay from honk
#define LOW_VOLT_LOW_BATT_BEEP				1000  // length of time it is honking for
#define LOW_VOLT_TIME_MAX_HORN_ON_TIME		10000

// amount of time to decide if you mean to honk, or only mean to ring the bell
#define BELL_DEBOUNCE_T 120 // change to 400 for quieter debugging

typedef enum {
	LOW_VOLT_STATE_INIT,          // 0
	LOW_VOLT_STATE_KILL,          // 1
	LOW_VOLT_STATE_CHECK_BELL,    // 2
	LOW_VOLT_STATE_CHECK_HORN0,   // 3
	LOW_VOLT_STATE_CHECK_HORN1,   // 4
	LOW_VOLT_STATE_END_BEEP,      // 5
	LOW_VOLT_STATE_DEAD           // 6
} LowVoltStates;


//Defines for I/O
#define VOLT_KILL_AC_PORT			PORTA
#define VOLT_KILL_AC_PIN			5
#define VOLT_KILL_AC_BIT			(1 << VOLT_KILL_AC_PIN)
#define VOLT_KILL_AC_CTRL			PORTA.PIN5CTRL

#define VOLT_KILL_ADC_PORT			PORTA
#define VOLT_KILL_ADC_PIN			7
#define VOLT_KILL_ADC_BIT			(1 << VOLT_KILL_ADC_PIN)
#define VOLT_KILL_ADC_CTRL			PORTA.PIN7CTRL

//Prototypes
void LowVoltKill_init(void);
void LowVoltKill_update(void);
