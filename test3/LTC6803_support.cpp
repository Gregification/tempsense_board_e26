#include "LTC6803_support.h"


//ltc.14
uint16_t ADCRangeTomV(uint16_t adc) {
    // mV = (adc - 512 ) * 1.5
    return ((adc - 512) * 3) >> 1;
}

/** ltc.17
 * calculates a 8 bit pec code from byte array
 * 
 * ltc uses a pec quotent ("PEC_POLY") of 0x7, equivilent to 0x8, with a seed of 0x41.
 *      - i think its just a 'x^4,3,2,1 + 1' polynomial when u consider how their funciton 
 *          works.
 *      LT paid them to figure the rest out + im not 
 *          => I trust them (aka I have no idea why they chose it).
*/
uint8_t pec8calc(uint8_t len, uint8_t* data) {

}

/**
 * general purpose pec8 calculator function
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
uint8_t pec8_calc(uint8_t quotient, uint8_t seed, uint8_t len, uint8_t const * data) {
    uint8_t remainder = seed;

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
