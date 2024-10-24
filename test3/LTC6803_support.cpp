#include "LTC6803_support.h"

#include <SPI.h>

//ltc.14
uint16_t adc_CV_to_V(uint16_t adc) {
    // mV = (adc - 512 ) * 1.5
    return ((adc - 512) * 3) >> 1;
}

/** 
 * calculates a 8 bit pec code from byte array
 * 
 * not using the datasheets process as described at ltc.17 because it sucks
 * we know the 
 * 
 * the LTC uses a 'x^8,2,1 + 1' polynomial with a seed of 0x41.
 *      LT paid them to pull that number out of their _ + im wasent
 *          => i trust the number
*/
uint8_t pec8calc(uint8_t len, uint8_t data[]) {

    uint8_t remainder = 0x41; 

    for (uint8_t din = 0; din < len; ++din) {
        //add the bit
        remainder ^= data[din];

        //recalc remainder
        for (uint8_t bit = 8; bit > 0; --bit) {

            if (remainder & (1<<7))
                remainder = (remainder << 1) ^ 0x7;
            else
                remainder <<= 1;
        }

    }

    return remainder;
}

void setCFG(uint8_t cfg[][6], uint8_t ic_num, uint8_t reg, uint8_t mask, uint8_t val)
{
    if(ic_num < 0){
        for(uint8_t i = 0; i < TOTAL_IC; i++){
            cfg[i][reg] = (cfg[i][reg] & (~mask)) | (val & mask);
        }
    } else {
        cfg[ic_num][reg] = (cfg[ic_num][reg] & (~mask)) | (val & mask);
    }
}

void spi_tx_command(LTC6803_Cmd cmd){
    spi_write(2, cmd.arr);
}

void spi_rx(uint8_t len, uint8_t arr[]){
    for(uint8_t i = 0; i < len; i++){
        arr[i] = SPI.transfer(SPI_TRANSFER_DUMMY);
    }
}

void spi_tx(uint8_t len, uint8_t const arr[]) {
    for(uint8_t i = 0; i < len; i++){
        SPI.transfer(arr[i]);
    }
}

void mask_unused_cells(uint8_t cfg[TOTAL_IC][6])
    for(int ic = 0; ic < cfg; ic++){
        //the cell mask ordering is kinda funky, so funky code

        setCFG(cfg, i, 2, 0xF0, used_cells[i] << 4);

        setCFG(cfg, i, 3, 0xFF, used_cells[i] >> 5);
    }
}

void discharge(uint8_t cfg[TOTAL_IC][6], uint8_t ic_num, uint16_t cells){
    
    // cells [1, 8]
    setCFG(cfg, ic_num, 1, 0xFF, cells);

    // cells [9, 12]
    setCFG(cfg, ic_num, 1, 0xFF, cells >> 9);
}
