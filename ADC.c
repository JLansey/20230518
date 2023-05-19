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

uint16_t ADC_DataExternalInput;
uint16_t ADC_DataDCInput;
uint16_t ADC_DataBattery1;
uint16_t ADC_DataBattery2;
uint16_t ADC_DataBattery3;

void (*ADC0FunctionPntr)(void);
void (*ADC1FunctionPntr)(void);

//local Functions
void ADC_ExternalInput(void);
void ADC_DCInput(void);
void ADC_Battery1(void);
void ADC_Battery2(void);
void ADC_Battery3(void);


//*--------------------------------------------------------------------------------------
//* Function Name       : ADC_Init
//* Object              : Analog Digital Converter Initialize
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

void ADC_Init(void)
{
	VREF.CTRLA = VREF_DAC0REFSEL_1V1_gc | VREF_ADC0REFSEL_1V1_gc;
	VREF.CTRLC = VREF_ADC1REFSEL_1V1_gc;
	
	
	//configure pins
	ADC_CHAN_EXT_IN_PORT.OUTCLR = ADC_CHAN_EXT_IN_BIT;
	ADC_CHAN_INPUT_PORT.OUTCLR = ADC_CHAN_INPUT_BIT;
	ADC_CHAN_BAT1_PORT.OUTCLR = ADC_CHAN_BAT1_BIT;
	ADC_CHAN_BAT2_PORT.OUTCLR = ADC_CHAN_BAT2_BIT;
	ADC_CHAN_BAT3_PORT.OUTCLR = ADC_CHAN_BAT3_BIT;

	//disable inputs
	ADC_CHAN_EXT_IN_CTRL = (4 << PORT_ISC0_bp);
	ADC_CHAN_INPUT_CTRL = (4 << PORT_ISC0_bp);
	ADC_CHAN_BAT1_CTRL = (4 << PORT_ISC0_bp);
	ADC_CHAN_BAT2_CTRL = (4 << PORT_ISC0_bp);
	ADC_CHAN_BAT3_CTRL = (4 << PORT_ISC0_bp);

	//enable ADC channels
	ADC_ENA_CHANNELS_PORT.DIRSET = ADC_ENA_CHANNELS_BIT;
	ADC_ENA_CHANNELS_PORT.OUTSET = ADC_ENA_CHANNELS_BIT;

	//Set up ADC 1
    ADC0.CTRLA = ADC_RESSEL_10BIT_gc; // 10-bit resolution
    ADC0.CTRLB = ADC_SAMPNUM_ACC16_gc; // 16 samples per read
    ADC0.CTRLC = ADC_REFSEL_INTREF_gc | ADC_SAMPCAP_bm; // Internal reference, low capacitance
	ADC0.CTRLC |= ADC_PRESC_DIV32_gc; // Peripheral clock / 32
	ADC0.CTRLA |= ADC_ENABLE_bm;

	////Set up ADC 1
    //ADC1.CTRLA = ADC_RESSEL_10BIT_gc; // 10-bit resolution
    //ADC1.CTRLB = ADC_SAMPNUM_ACC16_gc; // 16 samples per read
    //ADC1.CTRLC = ADC_REFSEL_INTREF_gc | ADC_SAMPCAP_bm; // Internal reference, low capacitance
	//ADC1.CTRLC |= ADC_PRESC_DIV32_gc; // Peripheral clock / 32
	//ADC1.CTRLA |= ADC_ENABLE_bm;
	//
	ADC0.MUXPOS = (ADC_CHAN_EXT_IN << ADC_MUXPOS_gp);
	ADC0.COMMAND = ADC_STCONV_bm;
//
	//ADC1.MUXPOS = (ADC_CHAN_BAT2 << ADC_MUXPOS_gp);
	//ADC1.COMMAND = ADC_STCONV_bm;

	ADC0FunctionPntr = ADC_DCInput;
	//ADC1FunctionPntr = ADC_ExternalInput;
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
		ADC0FunctionPntr();
		ADC0.COMMAND = ADC_STCONV_bm;
	}

	//if (ADC1.INTFLAGS & ADC_RESRDY_bm)
	//{
		//ADC1FunctionPntr();
		//ADC1.COMMAND = ADC_STCONV_bm;
	//}
}

void ADC_ExternalInput(void)
{
	ADC0.MUXPOS = (ADC_CHAN_EXT_IN << ADC_MUXPOS_gp);
	ADC_DataBattery3 = ADC0.RES;

	ADC0FunctionPntr = ADC_DCInput;
}

void ADC_DCInput(void)
{
	ADC0.MUXPOS = (ADC_CHAN_DC_IN << ADC_MUXPOS_gp);
	ADC_DataExternalInput = ADC0.RES;

	ADC0FunctionPntr = ADC_Battery1;
}

void ADC_Battery1(void)
{
	ADC0.MUXPOS = (ADC_CHAN_BAT1 << ADC_MUXPOS_gp);
	ADC_DataDCInput = ADC0.RES;

	ADC0FunctionPntr = ADC_Battery2;
}

void ADC_Battery2(void)
{
	ADC0.MUXPOS = (ADC_CHAN_BAT2 << ADC_MUXPOS_gp);
	ADC_DataBattery1 = ADC0.RES;

	ADC0FunctionPntr = ADC_Battery3;
}

void ADC_Battery3(void)
{
	ADC0.MUXPOS = (ADC_CHAN_BAT3 << ADC_MUXPOS_gp);
	ADC_DataBattery2 = ADC0.RES;

	ADC0FunctionPntr = ADC_ExternalInput;
}


//*--------------------------------------------------------------------------------------
//* Function Name       : ADC_ReadPin
//* Object              : Read one of analog pins
//* Input Parameters    : none
//* Output Parameters   : none
//*--------------------------------------------------------------------------------------

int ADC_ReadPin(uint8_t ain)
{
    // 6. Configure an input by writing to the MUXPOS bit field in the MUXPOS (ADCn.MUXPOS) register.
    ADC1.MUXPOS = (ain << ADC_MUXPOS_gp);

    // Start conversion
    ADC1.COMMAND = ADC_STCONV_bm;

    // Wait for result ready
    while (!(ADC1.INTFLAGS & ADC_RESRDY_bm)) {};

    // Result
    return ADC1.RES;
}
