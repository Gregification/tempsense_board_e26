/**
 * supportive functions for LTC6803
 * 
 * notes:
 *  - SPI related commands are from the prespective of the master
 *  - no SPI transfer because the LTC dosnet use it
 */

#ifndef __LTC6803_SUPPORT_H__
#define __LTC6803_SUPPORT_H__

#include "Environment.h"
#include "LTC6803_cmds.h"

#include <stdint.h>

/**
 * given a config array sized [TOTAL_IC]x[6], 
 *  sets the value of the masked vars to match the corrosponding bits in 'val' 
 *  while retaining the non masked values
 * @param ic_num stack index of the ic, -1 to apply to all
 */
void setCFG(uint8_t cfg[][6], uint8_t ic_num, uint8_t reg, uint8_t mask, uint8_t val);

/** converts the returned value of the cell voltage ADC to mV */
uint16_t adc_CV_to_mV(uint16_t adc_output);

/** LTC6803 pec generator */
uint8_t pec8calc(uint8_t length, uint8_t data[]);

/** sends command */
void spi_tx_command(LTC6803_Cmd command);

/** reads array */
void spi_rx(uint8_t len, uint8_t arr[]);

/** sends array. slave transmission is ignored */
void spi_tx(uint8_t len, uint8_t const arr[]);

/** enable/disables cell discharge. active high 
 * @param ic_num stack number of target ic, -1 for all of them
 * @param cells each bit represents the corrosponding cell 
 *      e.g: cell 1 is bit0, 
*/
void discharge(uint8_t cfg[TOTAL_IC][6], uint8_t ic_num, uint16_t cells);

#endif // __LTC6803_SUPPORT_H__
