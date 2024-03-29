// ********************************************************************************************* //
// ********************************************************************************************* //
// **************************************** SMARTPLANT ***************************************** //
// ********************************************************************************************* //
// ********************************************************************************************* //
// AUTOR:   Imanol Padillo
// DATE:    30/10/2022
// VERSION: 1.0

// Board: Arduino Pro Micro (Leonardo)
// Processor: AtMega32U4
//
// External module: SSD1306 (OLED screen)
//     SSD1306    | Arduino Pro Micro
// __________________________________
//        SDA     |       D2    (grey)
//        SCL     |       D3
//        Vcc     |       5V
//        GND     |      GND
//
// External module: (Humidity sensor)
//      FC-28     | Arduino Pro Micro
// __________________________________
//        A0      |       A0
//        Vcc     |       5V
//        GND     |      GND
//
// External module: LDR (light sensor)
//       LDR      | Arduino Pro Micro
// __________________________________
//       LDR      |       A1
//
// External module: DS18B20 (temperature sensor)
//     DS18B20    | Arduino Pro Micro
// __________________________________
//     DS18B20    |       D5
//
// External module: 3-pin switch
//  3-pin switch  | Arduino Pro Micro
// __________________________________
//      pin-1     |       D6
//      pin-2     |       D7
//      pin-3     |       D8
// External module: buzzer
//      buzzer    | Arduino Pro Micro
// __________________________________
//      buzzer    |       D4

// ********************************************************************************************* //
// INCLUDES 
// ********************************************************************************************* //
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ********************************************************************************************* // 
// CONSTANTS
// ********************************************************************************************* //
#define __DEBUG__
#define SCREEN_W                                128 // Oled screen - wide
#define SCREEN_H                                 64 // Oled screen - high
#define LOOP_DELAY                            10000 // Long delay time (ms) for every loop
#define LOOP_COUNTER_LIMIT                      360 // Buzzer will be activated if no water every
                                                    // LOOP_COUNTER_LIMIT x LONG_LOOP_DELAY seconds
#define GPIO_SWITCH_HUMIDITY                      6 // Switch pin for selecting threshold index for 
                                                    // humidty sensor (orange)
#define GPIO_SWITCH_LIGHT                         7 // Switch pin for selecting threshold index for 
                                                    // light sensor (blue)
#define GPIO_SWITCH_TEMPERATURE                   8 // Switch pin for selecting threshold index for 
                                                    // temperature sensor (grey)
#define GPIO_BUZZER                               4 // Buzzer
#define GPIO_HUMIDITY_SENSOR                     A0 // Output from humidity sensor
#define GPIO_LIGHT_SENSOR                        A1 // Output from light sensor
#define GPIO_TEMPERATURE_SENSOR                   5 // Output from temperature sensor
#define SENSOR_STATUS_L                           0 // Sensor status is low
#define SENSOR_STATUS_M                           1 // Sensor status is medium
#define SENSOR_STATUS_H                           2 // Sensor status is high
#define BUZZER_FREQ                            1000 // Buzzer frequency
#define BUZZER_DURATION                         200 // Buzzer duration (ms)

const int HUMIDITY_CALIBRATED_LIMITS[] = {512,200}; // Humidity sensor: dry and wet values
const int HUMIDITY_THRESHOLD_L[] =         {50,60}; // Humidity sensor (%): low thresholds 
const int HUMIDITY_THRESHOLD_H[] =         {75,85}; // Humidity sensor (%): high thresholds 
const int LIGHT_THRESHOLD_L[] =            {25,45}; // Light sensor (%): low thresholds
const int LIGHT_THRESHOLD_H[] =            {65,85}; // Light sensor (%): high thresholds
const float TEMPERATURE_OFFSET =                 0; // Temperature sensor: offset degrees
const int TEMPERATURE_THRESHOLD_L[] =      {18,20}; // Temperature sensor: low thresholds
const int TEMPERATURE_THRESHOLD_H[] =      {22,24}; // Temperature sensor: high thresholds

static const unsigned char PROGMEM  image_data_SubPict0[272] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙███████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙███████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙███████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙█∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙█∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙██∙█████∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙█∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙█∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙█∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙█∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙██∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙███∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙██∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙██∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙██∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 
    0x0f, 0x80, 0x00, 0x00, 0x1f, 0x80, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 
    0x1f, 0xc0, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x22, 0x20, 0x00, 0x00, 0x10, 0x40, 0x00, 0x00, 
    0x07, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x6f, 0xb0, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 
    0x07, 0x00, 0x00, 0x00, 0x10, 0x40, 0x00, 0x00, 0x22, 0x20, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 
    0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 
    0x05, 0x00, 0x00, 0x00, 0x0d, 0x80, 0x00, 0x00, 0x1d, 0xc0, 0x00, 0x00, 0x18, 0xc0, 0x00, 0x00, 
    0x18, 0xc0, 0x00, 0x00, 0x0d, 0x80, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM  image_data_SubPict_empty[128] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM image_data_SubPict10_light_L[192] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙███∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙███∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙████∙∙∙
    // ∙∙∙∙∙████∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙████∙∙∙∙
    // ∙∙∙∙∙∙██████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████████∙∙∙∙∙
    // ∙∙∙∙∙∙∙████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████████∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x01, 0xc0, 0x00, 0x70, 0x0e, 0x00, 
    0x07, 0x03, 0xc0, 0x00, 0x38, 0x1e, 0x00, 0x07, 0x87, 0x80, 0x00, 0x3c, 0x3c, 0x00, 0x03, 0xff, 
    0x00, 0x00, 0x1f, 0xf8, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0xfc, 0x00, 0x00, 
    0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM image_data_SubPict10_light_M[192] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████████∙∙∙∙∙∙
    // ∙∙∙∙∙∙██████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████████∙∙∙∙∙
    // ∙∙∙∙∙████∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙████∙∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙████∙∙∙
    // ∙∙∙∙███∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙███∙∙∙
    // ∙∙∙∙███∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙███∙∙∙
    // ∙∙∙∙███∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙███∙∙∙
    // ∙∙∙∙███∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙███∙∙∙
    // ∙∙∙∙∙███∙∙∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙████∙∙∙
    // ∙∙∙∙∙████∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙████∙∙∙∙
    // ∙∙∙∙∙∙██████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████████∙∙∙∙∙
    // ∙∙∙∙∙∙∙████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████████∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x78, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x03, 
    0xff, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x07, 0x87, 0x80, 0x00, 0x3c, 0x3c, 0x00, 0x07, 0x03, 0xc0, 
    0x00, 0x38, 0x1e, 0x00, 0x0e, 0x01, 0xc0, 0x00, 0x70, 0x0e, 0x00, 0x0e, 0x01, 0xc0, 0x00, 0x70, 
    0x0e, 0x00, 0x0e, 0x01, 0xc0, 0x00, 0x70, 0x0e, 0x00, 0x0e, 0x01, 0xc0, 0x00, 0x70, 0x0e, 0x00, 
    0x07, 0x03, 0xc0, 0x00, 0x38, 0x1e, 0x00, 0x07, 0x87, 0x80, 0x00, 0x3c, 0x3c, 0x00, 0x03, 0xff, 
    0x00, 0x00, 0x1f, 0xf8, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0xfc, 0x00, 0x00, 
    0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM image_data_SubPict10_light_H[192] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙███████████████████∙∙∙∙∙∙∙∙███████████████████∙∙
    // ∙∙████████∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙████████∙∙∙∙∙∙∙∙∙██∙∙
    // ∙∙████████∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙████████∙∙∙∙∙∙∙∙∙██∙∙
    // ∙∙████████∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙████████∙∙∙∙∙∙∙∙∙██∙∙
    // ∙∙██████████████∙∙∙██∙∙∙∙∙∙∙∙██████████████∙∙∙██∙∙
    // ∙∙██████████████∙∙∙██∙∙∙∙∙∙∙∙██████████████∙∙∙██∙∙
    // ∙∙██████████████∙∙∙████████████████████████∙∙∙██∙∙
    // ∙∙██████████████∙∙∙████████████████████████∙∙∙██∙∙
    // ∙∙██████████████████████████████████████████████∙∙
    // ∙∙██████████████████████████████████████████████∙∙
    // ∙∙██████████████████████████████████████████████∙∙
    // ∙∙███████████████████∙∙∙∙∙∙∙∙███████████████████∙∙
    // ∙∙███████████████████∙∙∙∙∙∙∙∙███████████████████∙∙
    // ∙∙███████████████████∙∙∙∙∙∙∙∙███████████████████∙∙
    // ∙∙███████████████████∙∙∙∙∙∙∙∙███████████████████∙∙
    // ∙∙███████████████████∙∙∙∙∙∙∙∙███████████████████∙∙
    // ∙∙███████████████████∙∙∙∙∙∙∙∙███████████████████∙∙
    // ∙∙███████████████████∙∙∙∙∙∙∙∙███████████████████∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x3f, 0xff, 0xf8, 0x07, 0xff, 0xff, 0x00, 0x3f, 0xc0, 0x18, 0x07, 0xf8, 0x03, 
    0x00, 0x3f, 0xc0, 0x18, 0x07, 0xf8, 0x03, 0x00, 0x3f, 0xc0, 0x18, 0x07, 0xf8, 0x03, 0x00, 0x3f, 
    0xff, 0x18, 0x07, 0xff, 0xe3, 0x00, 0x3f, 0xff, 0x18, 0x07, 0xff, 0xe3, 0x00, 0x3f, 0xff, 0x1f, 
    0xff, 0xff, 0xe3, 0x00, 0x3f, 0xff, 0x1f, 0xff, 0xff, 0xe3, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
    0x3f, 0xff, 0xf8, 0x07, 0xff, 0xff, 0x00, 0x3f, 0xff, 0xf8, 0x07, 0xff, 0xff, 0x00, 0x3f, 0xff, 
    0xf8, 0x07, 0xff, 0xff, 0x00, 0x3f, 0xff, 0xf8, 0x07, 0xff, 0xff, 0x00, 0x3f, 0xff, 0xf8, 0x07, 
    0xff, 0xff, 0x00, 0x3f, 0xff, 0xf8, 0x07, 0xff, 0xff, 0x00, 0x3f, 0xff, 0xf8, 0x07, 0xff, 0xff, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM image_data_SubPict11_humidity_L[192] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙
    // ∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙∙
    // ∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙
    // ∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙███████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███████∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙█████████∙∙∙∙∙∙∙∙∙∙∙∙∙██████████∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙███████████████████████████∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████████████████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙██∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙██∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙██∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙██∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x0f, 0x00, 
    0x00, 0x00, 0x00, 0x78, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x03, 0xe0, 0x00, 0x00, 
    0x03, 0xf0, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x7f, 0xc0, 0x01, 0xff, 0x80, 
    0x00, 0x00, 0x1f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 
    0x00, 0xe0, 0xc1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xc1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xe0, 
    0xc1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xc1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x70, 0x01, 0x80, 
    0x00, 0x00, 0x00, 0x00, 0x70, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x70, 0x03, 0x80, 0x00, 0x00, 
    0x00, 0x00, 0x78, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x38, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x3c, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x1e, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM image_data_SubPict11_humidity_M[192] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙███∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███∙∙∙∙
    // ∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙
    // ∙∙∙∙████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████∙∙∙∙∙
    // ∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙
    // ∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙███████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███████∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙█████████∙∙∙∙∙∙∙∙∙∙∙∙∙██████████∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙███████████████████████████∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████████████████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█████████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x1e, 0x00, 0x00, 0x00, 
    0x00, 0x3c, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x00, 0xf8, 
    0x00, 0x03, 0xe0, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 
    0x7f, 0xc0, 0x01, 0xff, 0x80, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xff, 
    0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static const unsigned char PROGMEM image_data_SubPict11_humidity_H[192] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███████████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████∙∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████∙∙∙∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████∙∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙███████████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██████████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfe, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x80, 0x00, 
    0x00, 0x00, 0x00, 0x7e, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x0f, 0xc0, 0x00, 0x00, 0x00, 
    0x00, 0xfc, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xf8, 
    0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x07, 0xc0, 
    0x00, 0x00, 0x00, 0x00, 0x7c, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x1f, 0x80, 0x00, 0x00, 
    0x00, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x1f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM image_data_SubPict20_light_L[128] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙████████∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙█████∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙██∙∙∙∙∙∙█████████∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙██∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙██████∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0x03, 0x80, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0c, 0x00, 
    0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x30, 0x00, 
    0x07, 0xe0, 0x60, 0x00, 0x00, 0x40, 0x60, 0x00, 0x00, 0xc0, 0xff, 0x80, 0x00, 0x80, 0x00, 0x00, 
    0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM image_data_SubPict21_temperature_L[128] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙█∙█∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙█∙∙█∙∙█∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙█∙∙∙███∙∙∙█∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙█∙∙█████∙∙█∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙█∙∙█████∙∙█∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙█∙∙∙███∙∙∙█∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x80, 0x00, 
    0x00, 0x04, 0x40, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x04, 0x40, 0x00, 
    0x00, 0x04, 0x40, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x04, 0x40, 0x00, 
    0x00, 0x04, 0x40, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x09, 0x20, 0x00, 
    0x00, 0x11, 0x10, 0x00, 0x00, 0x23, 0x88, 0x00, 0x00, 0x27, 0xc8, 0x00, 0x00, 0x27, 0xc8, 0x00, 
    0x00, 0x23, 0x88, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x08, 0x20, 0x00, 0x00, 0x07, 0xc0, 0x00, 
    0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM image_data_SubPict21_temperature_H[128] = {
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙█∙∙∙∙∙∙█∙∙∙∙∙∙█∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙█∙∙∙∙█∙█∙∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙█∙∙█∙∙∙█∙∙█∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙█∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙█∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙█∙∙█∙█∙█∙∙█∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙█∙∙∙█∙█∙█∙∙∙█∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙█∙∙∙∙█∙█∙█∙∙∙∙█∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙█∙∙∙∙∙█∙█∙█∙∙∙∙∙█∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙█∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙█∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙█∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█∙█∙█∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙█∙∙█∙∙█∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙█∙∙∙█∙∙∙█∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙█∙∙∙███∙∙∙█∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙█∙∙█████∙∙█∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙█∙∙█████∙∙█∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙█∙∙∙███∙∙∙█∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙█∙∙∙∙∙█∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙█████∙∙∙∙∙∙∙∙∙∙∙∙
    // ∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x81, 0x02, 0x00, 0x00, 0x42, 0x84, 0x00, 
    0x00, 0x24, 0x48, 0x00, 0x00, 0x05, 0x40, 0x00, 0x00, 0x05, 0x40, 0x00, 0x00, 0x25, 0x48, 0x00, 
    0x00, 0x45, 0x44, 0x00, 0x00, 0x85, 0x42, 0x00, 0x01, 0x05, 0x41, 0x00, 0x00, 0x05, 0x40, 0x00, 
    0x00, 0x05, 0x40, 0x00, 0x00, 0x05, 0x40, 0x00, 0x00, 0x05, 0x40, 0x00, 0x00, 0x09, 0x20, 0x00, 
    0x00, 0x11, 0x10, 0x00, 0x00, 0x23, 0x88, 0x00, 0x00, 0x27, 0xc8, 0x00, 0x00, 0x27, 0xc8, 0x00, 
    0x00, 0x23, 0x88, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x08, 0x20, 0x00, 0x00, 0x07, 0xc0, 0x00, 
    0x00, 0x00, 0x00, 0x00
};

// ********************************************************************************************* // 
// OBJECTS
// ********************************************************************************************* //
Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);
OneWire oneWireBus(GPIO_TEMPERATURE_SENSOR);
DallasTemperature temperature(&oneWireBus);
int previous_humidity_threshold_index=-1;
int previous_light_threshold_index=-1;
int previous_temperature_threshold_index=-1;
int loop_counter = 0;


void setup() {
#ifdef __DEBUG__
  Serial.begin(9600);
  delay(100);
  Serial.println("IntelliPlant version 1.0");
#endif
  // Init OLED screen at address 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
#ifdef __DEBUG__
    Serial.println("No OLED screen found");
#endif
    while (true);
  }

  // Configure GPIO
  pinMode(GPIO_BUZZER, OUTPUT);

  // Display intro
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(18, 32);
  display.println("SmartPlant v1.0");
  display.display();
  delay(3000);
}

void loop() {
  // Read sensors' data
  // humidity sensor response: HUMIDITY_CALIBRATED_LIMITS[0] = dry (eg. 512)
  //                           HUMIDITY_CALIBRATED_LIMITS[1] = wet (eg. 200)
  float humidity_raw_1 = (analogRead(GPIO_HUMIDITY_SENSOR)-HUMIDITY_CALIBRATED_LIMITS[1]);
  float humidity_raw_2 = (HUMIDITY_CALIBRATED_LIMITS[0]-HUMIDITY_CALIBRATED_LIMITS[1]);
  float humidity_raw = (1 - humidity_raw_1/humidity_raw_2)*100;
  int humidity = humidity_raw;
  if (humidity > 100) {
    humidity = 100;
  }else if (humidity < 0) {
    humidity = 0;
  }
  int light = analogRead(GPIO_LIGHT_SENSOR)*0.0978; // 1023 is 100%
  temperature.requestTemperatures();
#ifdef __DEBUG__  
  //Serial.print("Humidity_raw: ");
  //Serial.print(analogRead(GPIO_HUMIDITY_SENSOR));
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%;Light: ");
  Serial.print(light);
  Serial.print("%;Temperature: ");
  Serial.print(temperature.getTempCByIndex(0) - TEMPERATURE_OFFSET);
  Serial.print("°;");
#endif

  // Get sensors' thresholds
  int humidity_threshold_index=0;
  int light_threshold_index=0;
  int temperature_threshold_index=0;
  bool threshold_change=false;
  if (digitalRead(GPIO_SWITCH_HUMIDITY)==false){
    humidity_threshold_index = 1;
  }
  if (digitalRead(GPIO_SWITCH_LIGHT)==false){
    light_threshold_index = 1;
  }
  if (digitalRead(GPIO_SWITCH_TEMPERATURE)==false){
    temperature_threshold_index = 1;
  }
  // Check change in threshold value
  if ((previous_humidity_threshold_index != humidity_threshold_index) or
  (previous_light_threshold_index != light_threshold_index) or
  (previous_temperature_threshold_index != temperature_threshold_index)){
    threshold_change = true;
    previous_humidity_threshold_index = humidity_threshold_index;
    previous_light_threshold_index = light_threshold_index;
    previous_temperature_threshold_index = temperature_threshold_index;
#ifdef __DEBUG__
    Serial.print("Humidity threshold: ");
    Serial.print(humidity_threshold_index == 0 ? "LOW  " : "HIGH ");
    Serial.print(";Light threshold: ");
    Serial.print(light_threshold_index == 0 ? "LOW  " : "HIGH ");
    Serial.print(";Temperature threshold: ");
    Serial.print(temperature_threshold_index == 0 ? "LOW  " : "HIGH ");
    Serial.print(";");
#endif
  }

  // Get sensors' status
  int humidity_status = calculateSensorStatus(humidity, 
    HUMIDITY_THRESHOLD_L[humidity_threshold_index],
    HUMIDITY_THRESHOLD_H[humidity_threshold_index]);
  int light_status = calculateSensorStatus(light, 
    LIGHT_THRESHOLD_L[light_threshold_index], 
    LIGHT_THRESHOLD_H[light_threshold_index]);
  int temperature_status = calculateSensorStatus(temperature.getTempCByIndex(0) - TEMPERATURE_OFFSET, 
    TEMPERATURE_THRESHOLD_L[temperature_threshold_index], 
    TEMPERATURE_THRESHOLD_H[temperature_threshold_index]);
#ifdef __DEBUG__
  Serial.print("Humidity status: ");
  Serial.print(humidity_status);
  Serial.print(";Light status: ");
  Serial.print(light_status);
  Serial.print(";Temperature status: ");
  Serial.print(temperature_status);
  Serial.print(";");
#endif

  // Display info in oled
  if (threshold_change == false) {
    // Display picture
    displayPicture(humidity_status, light_status, temperature_status);
    // Display sensor values
    displaySensorValues(humidity, light, temperature.getTempCByIndex(0) - TEMPERATURE_OFFSET, humidity_status, 
      light_status, temperature_status);
  }
  else {
    // Display new thresholds
    displayThresholdValues(humidity_threshold_index, light_threshold_index,
      temperature_threshold_index);
  }

  // Activate buzzer if no water
  if (humidity_status == SENSOR_STATUS_L) {
    if (loop_counter >= LOOP_COUNTER_LIMIT){
      activateBuzzer();
      loop_counter = 0;
    }
    else{
      loop_counter += 1;
    }
  }
  else{
    loop_counter = 0;
  }
#ifdef __DEBUG__
  Serial.print("Loop counter: ");
  Serial.print(loop_counter);
  Serial.print(";");
#endif

  delay(LOOP_DELAY);
  
#ifdef __DEBUG__
  Serial.println("");
#endif

}


// ********************************************************************************************* // 
// FUNCTIONS
// ********************************************************************************************* //

// activateBuzzer: activate buzzer
void activateBuzzer(){
  tone(GPIO_BUZZER, 1000, 100);
#ifdef __DEBUG__
  Serial.print("Activate Buffer;");
#endif
}


// displayThresholdValues: display threshold values
void displayThresholdValues(int humidity_threshold_index, int light_threshold_index, 
  int temperature_threshold_index) {

  // Display icons
  display.clearDisplay();
  display.drawBitmap(0, 0, image_data_SubPict0, 30, 64, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Display humidity threshold values
  String text = "";
  text = (humidity_threshold_index == 0 ? "LOW  " : "HIGH ");
  text += " [" + String(int(HUMIDITY_THRESHOLD_L[humidity_threshold_index]));
  text += ", " + String(int(HUMIDITY_THRESHOLD_H[humidity_threshold_index])) + "] %";
  display.setCursor(18, 6);
  display.print(text);

  // Display light threshold values
  text = (light_threshold_index == 0 ? "LOW  " : "HIGH ");
  text += " [" + String(int(LIGHT_THRESHOLD_L[light_threshold_index]));
  text += ", " + String(int(LIGHT_THRESHOLD_H[light_threshold_index])) + "] %";
  display.setCursor(18, 27);
  display.print(text);

  // Display temperature threshold values
  text = (temperature_threshold_index == 0 ? "LOW  " : "HIGH ");
  text += " [" + String(int(TEMPERATURE_THRESHOLD_L[temperature_threshold_index]));
  text += ", " + String(int(TEMPERATURE_THRESHOLD_H[temperature_threshold_index])) + "] ";
  display.setCursor(18, 48);
  display.print(text);
  display.write(167);  //degrees symbol

  display.display();
}

  
// displaySensorValues: write sensor values over SubPict0
void displaySensorValues(int humidity_percentage, int light_percentage, int temperature, 
  int humidity_status, int light_status, int temperature_status) {

  // Display humidity data
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(14, 6);
  display.print(humidity_percentage);
  display.println("%");
  if (humidity_status==SENSOR_STATUS_L) {
    display.setCursor(18, 16);
    display.println("-");
  }
  else if (humidity_status==SENSOR_STATUS_H) {
    display.setCursor(18, 16);
    display.println("+");
  }

  // Display light data
  display.setCursor(14, 27);
  display.print(light_percentage);
  display.println("%");
  if (light_status==SENSOR_STATUS_L) {
    display.setCursor(18, 37);
    display.println("-");
  }
  else if (light_status==SENSOR_STATUS_H) {
    display.setCursor(18, 37);
    display.println("+");
  }

  // Display temperature data
  display.setCursor(14, 48);
  display.print(temperature);
  display.write(167);  //degrees symbol
  if (temperature_status==SENSOR_STATUS_L) {
    display.setCursor(18, 57);
    display.println("-");
  }
  else if (temperature_status==SENSOR_STATUS_H) {
    display.setCursor(18, 57);
    display.println("+");
  }
  
  display.display();
}


// calculateSensorStatus: calculates if sensor data is in status low, medium or high.
int calculateSensorStatus(int value, int thresholdL, int thresholdH) {
  int output = SENSOR_STATUS_M;
  if(value<=thresholdL){
    output = SENSOR_STATUS_L;
  }
  else if (value>=thresholdH){
    output = SENSOR_STATUS_H;
  }
  return output;
}

// generatePictureArray: concatenates subArrays so as to generate a picture array of 128x64.
//               x0          y0          w          h
// subPicture0:  0           0           30         64          
// subPicture10: 30+Xoffset  0 +Yoffset  50         25
// subPicture11: 30+Xoffset  25+Yoffset  50         25 
// subPicture20: 80+Xoffset  0 +Yoffset  30         25          
// subPicture21: 80+Xoffset  25+Yoffset  30         25 
//
// 0              30                  80             110
// ____________________________________________________    0
// |              |                    |              |
// |              |    SubPicture10    | SubPicture20 |
// |              |                    |              |
// | SubPicture0  _____________________________________   25
// |              |                    |              |
// |              |    SubPicture11    | SubPicture21 |
// |              |                    |              |
// ____________________________________________________   50
//
// Xoffset varies from 0 to 18
// Yoffset varies from 0 to 14

void displayPicture(int humidity_status, int light_status, int temperature_status){
  // Generate offsets
  int Xoffset=random(18);
  int Yoffset=random(14);
  
  // Clear the buffer
  display.clearDisplay();

  // Generate subPicture0
  display.drawBitmap(0, 0, image_data_SubPict0, 30, 64, SSD1306_WHITE);

  // Generate subPicture10
  if (light_status == SENSOR_STATUS_L){
    display.drawBitmap(30+Xoffset, 0+Yoffset, image_data_SubPict10_light_L, 50, 25, SSD1306_WHITE);
  } 
  else if(light_status == SENSOR_STATUS_H){
    display.drawBitmap(30+Xoffset, 0+Yoffset, image_data_SubPict10_light_H, 50, 25, SSD1306_WHITE);
  }
  else {
    display.drawBitmap(30+Xoffset, 0+Yoffset, image_data_SubPict10_light_M, 50, 25, SSD1306_WHITE);
  }

  // Generate subPicture11
  if (humidity_status == SENSOR_STATUS_L){
    display.drawBitmap(30+Xoffset, 25+Yoffset, image_data_SubPict11_humidity_L, 50, 25, SSD1306_WHITE);
  } 
  else if(humidity_status == SENSOR_STATUS_H){
    display.drawBitmap(30+Xoffset, 25+Yoffset, image_data_SubPict11_humidity_H, 50, 25, SSD1306_WHITE);
  }
  else {
    display.drawBitmap(30+Xoffset, 25+Yoffset, image_data_SubPict11_humidity_M, 50, 25, SSD1306_WHITE);
  }

  // Generate subPicture20
  if (light_status == SENSOR_STATUS_L){
    display.drawBitmap(80+Xoffset, 0+Yoffset, image_data_SubPict20_light_L, 30, 25, SSD1306_WHITE);
  } 
  else {
    display.drawBitmap(80+Xoffset, 0+Yoffset, image_data_SubPict_empty, 30, 25, SSD1306_WHITE);
  }

  // Generate subPicture21
  if (temperature_status == SENSOR_STATUS_L){
    display.drawBitmap(80+Xoffset, 25+Yoffset, image_data_SubPict21_temperature_L, 30, 25, SSD1306_WHITE);
  } 
  else if(temperature_status == SENSOR_STATUS_H){
    display.drawBitmap(80+Xoffset, 25+Yoffset, image_data_SubPict21_temperature_H, 30, 25, SSD1306_WHITE);
  }
  else {
    display.drawBitmap(80+Xoffset, 25+Yoffset, image_data_SubPict_empty, 30, 25, SSD1306_WHITE);
  }

  // Display picture
  display.display();
}
