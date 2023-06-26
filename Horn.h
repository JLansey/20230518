/*****************************************************************************************
**
**  Horn.h
**
**  Horn Functions for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

//defines for charger I/O pins
#define HORN_PORT		PORTB
#define HORN_PIN		0
#define HORN_BIT		(1 << HORN_PIN)

//Prototypes
void Horn_init(void);
void Horn_Bell(void);
void Horn_Enable(uint8_t Enable);
