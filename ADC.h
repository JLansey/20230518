/*****************************************************************************************
**
**  ADC.h
**
**  Analog to Digital Functions Interface Drivers
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

#ifndef _ADC_H_
#define _ADC_H_

//#defines for Analog Inputs
#define ADC_CHAN_EXT_IN			3
#define ADC_CHAN_DC_IN			2
#define ADC_CHAN_BAT1			1
#define ADC_CHAN_BAT2			4
#define ADC_CHAN_BAT3			5

#define ADC_ENA_CHANNELS_PORT	PORTB
#define ADC_ENA_CHANNELS_PIN	5
#define ADC_ENA_CHANNELS_BIT	(1 << ADC_ENA_CHANNELS_PIN)

#define ADC_CHAN_EXT_IN_PORT	PORTA
#define ADC_CHAN_EXT_IN_PIN		3
#define ADC_CHAN_EXT_IN_BIT		(1 << ADC_CHAN_EXT_IN_PIN)
#define ADC_CHAN_EXT_IN_CTRL	PORTA.PIN3CTRL

#define ADC_CHAN_INPUT_PORT		PORTA
#define ADC_CHAN_INPUT_PIN		2
#define ADC_CHAN_INPUT_BIT		(1 << ADC_CHAN_INPUT_PIN)
#define ADC_CHAN_INPUT_CTRL		PORTA.PIN2CTRL

#define ADC_CHAN_BAT1_PORT		PORTA
#define ADC_CHAN_BAT1_PIN		1
#define ADC_CHAN_BAT1_BIT		(1 << ADC_CHAN_BAT1_PIN)
#define ADC_CHAN_BAT1_CTRL		PORTA.PIN1CTRL

#define ADC_CHAN_BAT2_PORT		PORTA
#define ADC_CHAN_BAT2_PIN		4
#define ADC_CHAN_BAT2_BIT		(1 << ADC_CHAN_BAT2_PIN)
#define ADC_CHAN_BAT2_CTRL		PORTA.PIN4CTRL

#define ADC_CHAN_BAT3_PORT		PORTA
#define ADC_CHAN_BAT3_PIN		5
#define ADC_CHAN_BAT3_BIT		(1 << ADC_CHAN_BAT3_PIN)
#define ADC_CHAN_BAT3_CTRL		PORTA.PIN5CTRL


void ADC_Init(void);
void ADC_Update(void);
int ADC_ReadPin(uint8_t ain);

#endif /* _ADC_H_ */