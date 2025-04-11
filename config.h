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

// Define constants for the configuration modes
#define CONFIG_MODE_MINIBELL 0
#define CONFIG_MODE_MINI     1

// Time for mini honk extension (in ms)
#define MINI_HONK_EXTENSION_TIME 3

// Configuration Mode - keeping the same value for backward compatibility with preprocessor directives
// This will be updated during runtime from the main.c file
// Default configuration: 0 = MiniBell, 1 = Mini
#define CONFIG_MODE CONFIG_MODE_MINIBELL

#endif /* CONFIG_H */