#include "LTC6803_support.h"


//ltc.14
uint16_t ADCRangeTomV(uint16_t adc) {
    // mV = (adc - 512 ) * 1.5
    return ((adc - 512) * 3) >> 1;
}

/** 
 * calculates a 8 bit pec code from byte array using the datasheets process (ltc.17).
 * 
 * ltc uses a pec quotent ("PEC_POLY") of 0x7, with a seed of 0x41.
 *      - i think its just a 'x^8,2,1,-1 + 65' polynomial when u consider how their funciton 
 *          works.
 *      LT paid them to pull that number out of their _ + im not 
 *          => I trust them (aka I have no idea why they chose it).
*/
uint8_t pec8calc(uint8_t len, uint8_t* data) {
    //see ltc.17 for step numbers & variable names
    // not strictly following because its giving like a hardware bit-by-bit process
    //  thats not really viable for programming languages

    // #1
    uint8_t remainder = 0x41; 

    // #2
    for (uint8_t din = 0; din < len; ++din) {
        remainder ^= data[din];

        //the in0, in1, in2 stuff simplifies down to a slightly modded crc operation
        //  with the caveat that shift first then subtract.
        //  notice they dont actually tell us the quotent ( :( ) but its something 
        //  like 0x7, if you just look how its in0,1,2 (first 3 bits) being flipped,
        //  after the shift, so just '0x7 >> 1'. im missing somethign since I cant 
        //  explain the choice of shift first then subtract. I think this is either 
        //  a 4d power play going on for IP laws some evil critter thought up, or 
        //  just a way to detect 8 bit bursts within a crc-8 bit limit, since its 
        //  aligning the 8th bit while also factoring the -1th bit.
        // #LTHadItCommingForBadDocumentation

        for (uint8_t bit = 8; bit > 0; --bit) {

            // #2 & #3
            if (remainder & (1<<7))
                remainder = (remainder << 1) ^ 0x7;
            else // #3
                remainder <<= 1;
        }

    }

    return remainder;
}

/**
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
