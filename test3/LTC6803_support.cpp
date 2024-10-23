#include "LTC6803_support.h"

//ltc.14
uint16_t ADCRangeTomV(uint16_t adc) {
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
uint8_t pec8calc(uint8_t len, uint8_t* data) {

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

