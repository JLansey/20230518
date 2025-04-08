/*****************************************************************************************
**
**  Charger.h
**
**  Charger Functions for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

#ifndef CHARGER_H
#define CHARGER_H

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

#endif /* CHARGER_H */