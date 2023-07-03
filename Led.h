/*****************************************************************************************
**
**  Led.h
**
**  LED Functions for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

//LED timings
#define TIME_LED_STATUS_HEARTBEAT	250      //time x 1ms

//defines for LED I/O pins
#define LED_RED_PORT PORTB
#define LED_RED_PIN 5
#define LED_RED_BIT (1 << LED_RED_PIN)

#define LED_GREEN_PORT PORTB
#define LED_GREEN_PIN 4
#define LED_GREEN_BIT (1 << LED_GREEN_PIN)

//Prototypes
void LED_init(void);
void LED_Red(uint8_t Enable);
void LED_Green(uint8_t Enable);
void LED_update(void);
void blinker(uint8_t n);