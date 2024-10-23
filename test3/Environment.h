/**
 * custom variables to make adjustments easier
 */

#ifndef __ENVIROMENT_H__
#define __ENVIROMENT_H__

//--------------------------------------------------------------------
// general settings
//  
// change what ever u want
//--------------------------------------------------------------------

    /** total number of LTCs daisy chained together */
    #define TOTAL_IC      1
    
    /** Chip Select pin to be used in SPI.
     *   - requires a modified "LTC6803.cpp" file
     * adafruit pins seen here (the dark turquoise color) : https://cdn-learn.adafruit.com/assets/assets/000/111/275/original/adafruit_products_Adafruit_Feather_M4_CAN_Pinout.png?1651520615
    */
    #define CS_PIN 4

    /** undervoltage threshold in mV 
     * uint8_t
    */
    #define UNDERVOLTAGE_THRESH_mV   2800

    /** overvoltage threshold in mV 
     * uint8_t
    */
    #define OVERVOLTAGE_THRESH_mV    4500


//--------------------------------------------------------------------
// advanced settings
// 
// please skim the datasheet before changing things.
//--------------------------------------------------------------------

    /** The value to write to the under voltage register 
     * ltc.24 
     * comparison voltage = (Vuv – 31) • 16 • 1.5mV
    */
    #define UNDERVOLTAGE_THRESH_VALUE ((uint8_t)(UNDERVOLTAGE_THRESH_mV / 16 / 1.5 + 31))

    /** The value to write to the over voltage register 
     * ltc.24 
     * comparison voltage = (Vov – 32) • 16 • 1.5mV
    */
    #define OVERVOLTAGE_THRESH_VALUE ((uint8_t)(UNDERVOLTAGE_THRESH_mV / 16 / 1.5 + 32))

#endif // __ENVIROMENT_H__
