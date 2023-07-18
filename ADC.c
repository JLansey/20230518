/*****************************************************************************************
**
**  ADC.c
**
**  Analog to Digital Functions Interface Drivers
**
**  2022 CPU Ready Inc
**
******************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Timer.h"
#include "ADC.h"

uint16_t ADC_DataBattery;

void (*ADC0FunctionPntr)(void);

//local Functions
void ADC_Battery(void);


//*--------------------------------------------------------------------------------------
//* Function Name       : ADC_Init
//* Object              : Analog Digital Converter Initialize
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void ADC_Init(void)
{
	VREF.CTRLA = VREF_DAC0REFSEL_1V1_gc | VREF_ADC0REFSEL_1V1_gc;
	
	//configure pins
	ADC_CHAN_BAT_PORT.OUTCLR = ADC_CHAN_BAT_BIT;

	//disable inputs
	ADC_CHAN_BAT_CTRL = (4 << PORT_ISC0_bp);

	//Set up ADC 0
    ADC0.CTRLA = ADC_RESSEL_10BIT_gc; // 10-bit resolution
    ADC0.CTRLB = ADC_SAMPNUM_ACC16_gc; // 16 samples per read
    ADC0.CTRLC = ADC_REFSEL_INTREF_gc | ADC_SAMPCAP_bm; // Internal reference, low capacitance
	ADC0.CTRLC |= ADC_PRESC_DIV32_gc; // Peripheral clock / 32
	ADC0.CTRLA |= ADC_ENABLE_bm;

	ADC0.MUXPOS = (ADC_CHAN_BAT << ADC_MUXPOS_gp);
	ADC0.COMMAND = ADC_STCONV_bm;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : ADC_Update
//* Object              : Analog Digital Converter Update
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void ADC_Update(void)
{
	if (ADC0.INTFLAGS & ADC_RESRDY_bm)
	{
		ADC_DataBattery = (ADC0.RES >> 4);
		ADC0.COMMAND = ADC_STCONV_bm;
	}
}
