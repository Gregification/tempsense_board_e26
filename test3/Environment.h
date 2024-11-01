/**
 * adjustable parameters to quickly reconfigure the LTC68031 code.
 * 
 * - if errors appear in IN THIS FILE after changing something 
 *      that is intentional, go 'fix' it.
 * - keep units in the macros so that limits are clear
 */

#ifndef ENVIROMENT_H__
#define ENVIROMENT_H__

#include <stdint.h>

//--------------------------------------------------------------------
// general settings
//  
// change what ever u want
//--------------------------------------------------------------------

#define SERIAL_BAUD     9600

/** total number of LTCs daisy chained together */
#define TOTAL_IC        1

/** Chip Select pin to be used in SPI.
 *   - requires a modified "LTC6803.cpp" file
 * adafruit pins seen here (the dark turquoise color) : https://cdn-learn.adafruit.com/assets/assets/000/111/275/original/adafruit_products_Adafruit_Feather_M4_CAN_Pinout.png?1651520615
*/
#define CS_PIN                      4

#define STATUS_LED_PIN              10

#define LOG_DUMP_PIN                11

#define LOG_DELETE_PIN              15

#define SPI_MISO_PIN                23

// /** minimum time between measurements of any single cell */
#define MEASUREMENT_PERIOD_mS       ((unsigned long)1000)

/** the max edges per second
 * special board considerations needed for speeds above 1Mhz
 */
#define LTC6803_SPI_CLK_SPEED       ((uint32_t)10000)

// /** dummy value to transfer over spi when intending to read */
#define SPI_TRANSFER_DUMMY          0xFF

/** discharge cells during voltage measurements*/
// #define DISCHARGE_WHEN_ADC

/** use 10 cell mode when possible
 * pro: LTC will save some power and time
 * con: LTC will not be able to measure 12 cells without resetting the configutaion
 */
// #define CELL10_MODE_IF_POSSIBLE

// #define LOG_ENABLED

#define LOG_FILE_NAME               "log.csv"

//--------------------------------------------------------------------
// variable stuff
//--------------------------------------------------------------------

#define ADC_to_mV

/**cells that are being used for each layer of the stack
 * e.g : only using 2 cells on the 5th chip in the stack
 *  meaning that cells 0 and 1 are used,
 *    "used_cells[4] = 2;"
*/
const uint8_t used_cells[TOTAL_IC] = {
    12
};

/** received cell codes */
extern uint16_t cell_codes     [TOTAL_IC][12];

/** received temp codes */
extern uint16_t temp_codes     [TOTAL_IC][3];

/** transmitted configurations */
extern uint8_t tx_cfg          [TOTAL_IC][6];

/** received configuraitons */
extern uint8_t rx_cfg          [TOTAL_IC][7];  // idk why this is larger than tx_cfg

#endif // ENVIROMENT_H__
