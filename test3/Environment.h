/**
 * adjustable parameters to quickly reconfigure the code.
 * 
 * - if errors appear in IN THIS FILE after changing something 
 *      that is intentional, go 'fix' it.
 * - keep units in the macros so that limits are clear
 */

#ifndef __ENVIROMENT_H__
#define __ENVIROMENT_H__

#include <array>

//--------------------------------------------------------------------
// general settings
//  
// change what ever u want
//--------------------------------------------------------------------

    /** total number of LTCs daisy chained together */
    #define TOTAL_IC    1
    
    /** Chip Select pin to be used in SPI.
     *   - requires a modified "LTC6803.cpp" file
     * adafruit pins seen here (the dark turquoise color) : https://cdn-learn.adafruit.com/assets/assets/000/111/275/original/adafruit_products_Adafruit_Feather_M4_CAN_Pinout.png?1651520615
    */
    #define CS_PIN      4

    /** measurements taken once every period
     * when not measuring, internal discharge is disabled.
     */
    #define MEASUREMENT_PERIOD_mS       ((unsigned long)200)

    /** undervoltage threshold in mV */
    #define UNDERVOLTAGE_THRESH_mV      ((uint16_t)2800)

    /** overvoltage threshold in mV */
    #define OVERVOLTAGE_THRESH_mV       ((uint16_t)4500)

    /** the max clock speed
     * special bord considerations needed for speeds above 1Mhz
     */
    #define LTC6803_SPI_CLK_SPEED       ((uint32_t)800'000)

    /** CDC, ltc.24, 3 value*/
    #define CDC                         2

    /** dummy value to transfer over spi when intending to read */
    #define SPI_TRANSFER_DUMMY          ((uint8_t)0xFF)

//--------------------------------------------------------------------
// sensitive settings
// please skim the datasheet before changing theses macros.
//--------------------------------------------------------------------

    /** The value to write to the under voltage register 
     * ltc.24 
     * comparison voltage = (Vuv – 31) • 16 • 1.5mV
    */
    #define UNDERVOLTAGE_THRESH_VALUE   ((uint8_t)(UNDERVOLTAGE_THRESH_mV / 16 / 1.5 + 31))

    /** The value to write to the over voltage register 
     * ltc.24 
     * comparison voltage = (Vov – 32) • 16 • 1.5mV
    */
    #define OVERVOLTAGE_THRESH_VALUE    ((uint8_t)(UNDERVOLTAGE_THRESH_mV / 16 / 1.5 + 32))

    /** cell_v_measure_time: duration of the cell actually measured after a adc command */
    #if CDC < 5
        #define CELL_V_MEASURE_TIME_mS  13
    #elif CDC < 8
        #define CELL_V_MEASURE_TIME_mS  21
    #endif


//--------------------------------------------------------------------
// stuff, change as needed
//--------------------------------------------------------------------

    /**cells that are being used  (program specific)
     *  ltc requires that certiain steps be taken for each unused cell.
     * 
     * cell enabled : 1
     * cell disabled: 0
     * 
     * e.g : ic #4 only has cell 2 & 1 enabled
     *  used_cells[4] = 0b11; //pesudo code
    */
    const std::array<uint16_t, TOTAL_IC> used_cells = {
        0xFFF,
        // 0b0000'0000'1111,
    };

    /** received cell codes */
    uint16_t cell_codes     [TOTAL_IC][12];

    /** received temp codes */
    uint16_t temp_codes     [TOTAL_IC][3];
    
    /** transmitted configurations */
    uint8_t tx_cfg          [TOTAL_IC][6];

    /** received configuraitons */
    uint8_t rx_cfg          [TOTAL_IC][7];  // idk why this is larger than tx_cfg

#endif // __ENVIROMENT_H__


//--------------------------------------------------------------------
// tests
//--------------------------------------------------------------------

#if CDC < 0 || CDC > 7
    #error CDC value must be in range [0,7]
#elif CDC == 0
    #error CDC 0 means no measurements, we want measurements
#endif
