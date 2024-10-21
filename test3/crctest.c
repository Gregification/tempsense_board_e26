/**
 * test if the crc funcitons match LT's calculations
 * 
 * tested in WSL Ubuntu 22.04
 * compile:
 *    gcc crctest.c --std=c99
 */
//-----------------------------------------------------------------------
// parameters
//-----------------------------------------------------------------------
// #define SHOW_SUCESSFUL_TESTS

#define SHOW_PROGRESS

// anything over than 100 is enough
#define NUM_TESTS ((unsigned long)1e5)

#define MAX_LEN ((uint8_t)40)

// #define RAND_SEED 13246
#define RAND_SEED time(NULL)

//-----------------------------------------------------------------------


#include <stdlib.h> // malloc
#include <stdint.h> // uint8_t
#include <stdio.h>  // printf
#include <time.h>   // time

// LT's crc function
#define PEC_POLY 7
uint8_t pec8_calc(uint8_t len, uint8_t const * data);

// alternative crc function
uint8_t pec8calc(uint8_t len, uint8_t const * data);

int main(){
    srand(RAND_SEED);

    uint8_t * arr = (uint8_t*)malloc(MAX_LEN * sizeof(uint8_t));

    for(unsigned long test_cases = 1; test_cases <= NUM_TESTS; test_cases++){
        uint8_t len = rand() % MAX_LEN + 1;
        for(int i = 0; i < len; i++){
            arr[i] = rand();
        }

        uint8_t result_expected = pec8_calc(len, arr);
        uint8_t result_actual = pec8calc(len, arr);

        #ifdef SHOW_SUCESSFUL_TESTS    
        printf("test %3ld. len: %3d expected: %8x , calculated: %8x\n",
                test_cases,
                len,
                result_expected,
                result_actual
            ); 
        #endif


        #ifdef SHOW_PROGRESS
        if(test_cases % (NUM_TESTS / 5) == 0){
            printf("%ld cases passed...\n", test_cases);
        }
        #endif

        if(result_expected != result_actual){
            printf("test %3ld. len: %3d expected: %8x , calculated: %8x\n",
                test_cases,
                len,
                result_expected,
                result_actual
            ); 
            printf("test failed\n");
            goto exit;
        }
    }

    printf("all cases passed\n");

exit:
    free(arr);

    return 0;
}


//Function that calculates PEC byte
uint8_t pec8_calc(uint8_t len, uint8_t const * data)
{

  uint8_t  remainder = 0x41;//PEC_SEED;


  /*
   * Perform modulo-2 division, a byte at a time.
   */
  for (int byte = 0; byte < len; ++byte)
  {
    /*
     * Bring the next byte into the remainder.
     */
    remainder ^= data[byte];

    /*
     * Perform modulo-2 division, a bit at a time.
     */
    for (uint8_t bit = 8; bit > 0; --bit)
    {
      /*
       * Try to divide the current data bit.
       */
      if (remainder & 128)
      {
        remainder = (remainder << 1) ^ PEC_POLY;
      }
      else
      {
        remainder = (remainder << 1);
      }
    }
  }

  /*
   * The final remainder is the CRC result.
   */
  return (remainder);

}

uint8_t pec8calc(uint8_t len, uint8_t const * data) {
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
        //  notice they dont actually tell us the **** quotent but its something 
        //  like 0x7, if you just look how its in0,1,2 (first 3 bits) being flipped,
        //  after the shift, so just '0x7 >> 1'. im missing somethign since I cant 
        //  explain the choice of shift first then subtract. I think this is either 
        //  a 4d power play going on for IP laws some evil critter thought up. Or 
        //  it might be a way to detect bursts of 8 bit wide, since its aligning the
        //  8th bit while also factoring the -1th bit.
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
