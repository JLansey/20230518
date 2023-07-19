/*****************************************************************************************
**
**  Charger.h
**
**  Charger Functions for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

//defines for Charger Timing
#define CHARGE_COUNT_1_SECOND	1000
#define CHARGE_FULL_MAX_MINUTES 10
#define CHARGE_FULL_MAX_SECONDS (CHARGE_FULL_MAX_MINUTES * 60) 	//Convert Minutes to Max seconds
#define CHARGE_LEVEL_CHARGED	0xd0							//ADC count when charge is finished

//defines for charger I/O pins
#define CHARGER_PWR_GOOD_PORT	PORTA
#define CHARGER_PWR_GOOD_PIN	4
#define CHARGER_PWR_GOOD_BIT	(1 << CHARGER_PWR_GOOD_PIN)
#define CHARGER_PWR_GOOD_CTRL	PORTA.PIN4CTRL

#define CHARGER_STATUS_PORT		PORTC
#define CHARGER_STATUS_PIN		1
#define CHARGER_STATUS_BIT		(1 << CHARGER_STATUS_PIN)
#define CHARGER_STATUS_CTRL		PORTC.PIN1CTRL

//Prototypes
void Charger_init(void);
void Charger_update(void);
void Charger_TimerReset(void);
