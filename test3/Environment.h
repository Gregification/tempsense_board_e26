/**
 * adjustable parameters to quickly reconfigure the LTC68031 code.
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

/** minimum time between measurements of any single cell */
#define MEASUREMENT_PERIOD_mS       ((unsigned long)1000)

/** the max clock speed
 * special board considerations needed for speeds above 1Mhz
 */
#define LTC6803_SPI_CLK_SPEED       ((uint32_t)950'000)

/** dummy value to transfer over spi when intending to read */
#define SPI_TRANSFER_DUMMY          ((uint8_t)0xFF)

/** discharge cells during voltage measurements*/
#define DISCHARGE_WHEN_ADC

/** use 10 cell mode when possible
 * pro: LTC will save some power and time
 * con: LTC will not be able to measure 12 cells without resetting the configutaion
 */
#define CELL10_MODE_IF_POSSIBLE

//--------------------------------------------------------------------
// variable stuff
//--------------------------------------------------------------------

/**cells that are being used for each layer of the stack
 * e.g : only using 2 cells on the 5th chip in the stack
 *  meaning that cells 0 and 1 are used,
 *    "used_cells[4] = 2;"
*/
const std::array<uint8_t, TOTAL_IC> used_cells = {
    12
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
