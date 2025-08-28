#ifndef GUARD_CONFIG_HPP
#define GUARD_CONFIG_HPP

#include <cstdio>
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

// Reasonable config stuff ~~~~~~~~~~~~~~~~~~~~~~

#define KEY_COUNT 35
#define FUNCTION_LAYERS 2

#define DIRECT_MULTIPLEXER_PIN_ENABLE_MODE // temporary 

// Analogue pins : ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define ADC1 A0

// Neopixels ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define NEOPIXEL_DATA_PIN D16
#define NEOPIXEL_COUNT 35

// Constants ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef uint8_t NORMALISED_ADC_VAL;
#define MAX_NORMALISED_ADC_VAL 255

#define MAX_ANALOG_VALUE 4096
#define MIN_ANALOG_VALUE 0
#define ANALOGUE_READ_RESOLUTION 12 // number of bits of accuracy

// Multiplexer IO pins ~~~~~~~~~~~~~~~~~~~~~~~~~~
#define MTP_BIN_PIN_1 D0 // These directly controll each analoge input multiplexer
#define MTP_BIN_PIN_2 D1
#define MTP_BIN_PIN_3 D2
#define MTP_BIN_PIN_4 D3

                                           
// Each analog multiplexer controlled by direct digital pin
#ifdef DIRECT_MULTIPLEXER_PIN_ENABLE_MODE 
#define MTP_1_ENABLE_PIN D4 // These enable the multiplexer disable pins 
#define MTP_2_ENABLE_PIN D5 // Each one enables 16 more pins 
#define MTP_3_ENABLE_PIN D6
#define MTP_4_ENABLE_PIN D7
#define MTP_5_ENABLE_PIN D8
#define MTP_6_ENABLE_PIN D9

#endif


// Another multiplexer controlls which multiplexer is enabled at any given time
#ifdef MULTIPLEXER_CONTROL_MULTIPLEXER 
#define MTP_CONTROLL_BIN_1 D4 // This controlls 16 multiplexers so 256 keys max
#define MTP_CONTROLL_BIN_2 D5 
#define MTP_CONTROLL_BIN_3 D6
#define MTP_CONTROLL_BIN_4 D7

#endif

#endif // include guard
