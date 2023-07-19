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
#define ADC_CHAN_BAT			7

#define ADC_CHAN_BAT_PORT		PORTA
#define ADC_CHAN_BAT_PIN		7
#define ADC_CHAN_BAT_BIT		(1 << ADC_CHAN_BAT_PIN)
#define ADC_CHAN_BAT_CTRL		PORTA.PIN7CTRL


void ADC_Init(void);
void ADC_Update(void);

#endif /* _ADC_H_ */