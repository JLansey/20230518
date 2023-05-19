/*****************************************************************************************
**
**  Timer.h
**
**  Timer Functions for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

void RTC_init(void);

uint16_t RTC_getTick(void);

void RTC_delayMS(uint16_t delay);