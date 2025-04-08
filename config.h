/*****************************************************************************************
**
**  Config.h
**
**  Configuration parameters for Tiny1616
**
**  2023 CPU Ready Inc
**
******************************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

// Configuration Mode
// 0 = MiniBell (default) - Use bell sound when button is released
// 1 = Mini - Use short honk extension (3ms) when button is released
#define CONFIG_MODE 0

// Define constants for the configuration modes
#define CONFIG_MODE_MINIBELL 0
#define CONFIG_MODE_MINI     1

// Time for mini honk extension (in ms)
#define MINI_HONK_EXTENSION_TIME 3

#endif /* CONFIG_H */