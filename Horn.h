/*****************************************************************************************
**
**  Horn.h
**
**  Horn Functions for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

#ifndef HORN_H
#define HORN_H

//defines for charger I/O pins
#define HORN_PORT		PORTB
#define HORN_PIN		0
#define HORN_BIT		(1 << HORN_PIN)

#define HORN_CPU_CLOCK	20000000UL
#define MIN_FREQ 1			// Minimum frequency in Hz
#define MAX_FREQ 20000		// Maximum frequency in Hz

typedef enum {
	HORN_OFF,  // 0
	HORN_ON,   // 1
	BELL,  // 2
	BELL_LOWVOLT, // 3
	BELL_CHARGING // 4
} SpeakerState;

typedef struct
{
	uint8_t TimeNextStep;	// in mS
	uint16_t frequency;		// In Hertz
	uint8_t duty_cycle;		// In percentage (0-100)
} PWMSetting;


//Prototypes
void Bell_Init();
void Horn_Enable(uint8_t Enable);
uint8_t Bell_Update(SpeakerState speaker_state);

#endif /* HORN_H */