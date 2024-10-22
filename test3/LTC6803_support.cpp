#include "LTC6803_support.h"


//ltc.14
uint16_t ADCRangeTomV(uint16_t adc) {
    // mV = (adc - 512 ) * 1.5
    return ((adc - 512) * 3) >> 1;
}


/** 
 * calculates a 8 bit pec code from byte array using the datasheets process (ltc.17).
 * 
 * ltc defines a polynomial ("PEC_POLY") of 0x7, with a seed of 0x41.
 *      - effectively acts as a 'x^8,2,1 + 1' polynomial, with a seed of 0x41
 *      LT paid them to pull that number out of their _ + im not 
 *          => I trust them (aka I have no idea why they chose it).
*/
uint8_t pec8calc(uint8_t len, uint8_t* data) {
    //see ltc.17
    // not strictly following because its giving like a hardware bit-by-bit process
    //  thats not really viable for programming languages
    // we know hte poly anyways, going to ignore their process

    uint8_t remainder = 0x41; 

    for (uint8_t din = 0; din < len; ++din) {
        remainder ^= data[din];

        for (uint8_t bit = 8; bit > 0; --bit) {

            if (remainder & (1<<7))
                remainder = (remainder << 1) ^ 0x7;
            else
                remainder <<= 1;
        }

    }

    return remainder;
}

/**
 * DOES NOT WORK, i forgo to account for the offset. i ll fix it later
 * general purpose pec8 calculator function.
 *  does not do width extension.
 * 
 * far slower than it can be, because its made to handle any possible
 *   quotient. will greatly speed it up if you can know the highest bit index of 
 *   the quotient in advance.
 * 
 * ben eater 
 *      - complete theory : https://youtu.be/izG7qT0EpBw?t=829
 *      - tldr theory : https://youtu.be/sNkERQlK8j8?t=1146
 *      - tldr tldr : division as such works because of binary finite field rules
*/
uint8_t pec8_calc_general(uint8_t quotient, uint8_t remainder, uint8_t len, uint8_t const * data) {

    uint8_t highestBit = quotient;//highest bit of quotent
    // cant find a compiler indipendent way to do this, so done manually
    for(uint8_t i = 0; i <= 8; i++) {
        if(highestBit == 0){
            if(i != 0) 
                highestBit = 1 << (i-1);
            break;
        }

        highestBit >>= 1;
    }

    //crc calculation
    for(; len >= 0; --len) {
        // add to remainder
        remainder ^= data[len];

        //divide to find new remainder
        //do the shifty & subtract across the value
        for(uint8_t i = 0; i < 8; i++) {

            //if >= than remainder : subtract
            if(remainder & highestBit)
                remainder ^= quotient;

            remainder <<= 1;
        }
    }

    return remainder;
}
