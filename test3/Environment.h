/**
 * custom variables to make adjustments easier
 */

#ifndef __ENVIROMENT_H__
#define __ENVIROMENT_H__

    /** total number of LTCs dasi chained together */
    #define TOTAL_IC      1
    
    /** Chip Select pin to be used in SPI.
     *   - requires a modified "LTC6803.cpp" file
     * adafruit pins seen here (the dark turquoise color) : https://cdn-learn.adafruit.com/assets/assets/000/111/275/original/adafruit_products_Adafruit_Feather_M4_CAN_Pinout.png?1651520615
    */
    #define CS_PIN 4

    /** undervoltage threshold in mV */
    #define UNDERVOLTAGE_THRESHOLD_mV   2800

    /** overvoltage threshold in mV */
    #define OVERVOLTAGE_THRESHOLD_mV    4500
    
#endif // __ENVIROMENT_H__
