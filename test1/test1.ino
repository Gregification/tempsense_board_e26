/**
  baselined form the DC1651A.ino file1

  LTC6803-1 datasheet used : https://www.analog.com/media/en/technical-documentation/data-sheets/680313fa.pdf
  
  notes:
    - ltc6803 is non programmable. the library is just wrapper on a spi.
    - cell balancing is done manually by toggling the S pins as needed.
        - pg12
    - there isnt much to the ic, everything the header has all there is.
*/

// imports from the "DC1651A.ino" file
#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"           //"hardware definitions for the Linduino"
#include "LT_SPI.h"             //"Routines to communicate with ATmega328P's hardware SPI port."
#include "UserInterface.h"
#include "LTC68031.h"
#include <SPI.h>
#include "LTC6803_cmds.h"

const uint8_t TOTAL_IC = 1;

uint16_t cell_codes[TOTAL_IC][12];
/*!<
  The cell codes will be stored in the cell_codes[][12] array in the following format:

  |  cell_codes[0][0]| cell_codes[0][1] |  cell_codes[0][2]|    .....     |  cell_codes[0][11]|  cell_codes[1][0] | cell_codes[1][1]|  .....   |
  |------------------|------------------|------------------|--------------|-------------------|-------------------|-----------------|----------|
  |IC1 Cell 1        |IC1 Cell 2        |IC1 Cell 3        |    .....     |  IC1 Cell 12      |IC2 Cell 1         |IC2 Cell 2       | .....    |
****/
uint16_t temp_codes[TOTAL_IC][3];
/*!<
 The Temp codes will be stored in the temp_codes[][3] array in the following format:

 |  temp_codes[0][0]| temp_codes[0][1]|temp_codes[0][2] | temp_codes[1][0]| temp_codes[1][1]|   .....   |
 |------------------|-----------------|-----------------|-----------------|-----------------|-----------|
 |IC1 Temp1         |IC1 Temp2        |IC1 ITemp        |IC2 Temp1        |IC2 Temp2        |  .....    |
*/
uint8_t tx_cfg[TOTAL_IC][6];
/*!<
  The tx_cfg[][6] stores the LTC6803 configuration data that is going to be written
  to the LTC6803 ICs on the daisy chain. The LTC6803 configuration data that will be
  written should be stored in blocks of 6 bytes. The array should have the following format:

 |  tx_cfg[0][0]| tx_cfg[0][1] |  tx_cfg[0][2]|  tx_cfg[0][3]|  tx_cfg[0][4]|  tx_cfg[0][5]| tx_cfg[1][0] |  tx_cfg[1][1]|  tx_cfg[1][2]|  .....    |
 |--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|-----------|
 |IC1 CFGR0     |IC1 CFGR1     |IC1 CFGR2     |IC1 CFGR3     |IC1 CFGR4     |IC1 CFGR5     |IC2 CFGR0     |IC2 CFGR1     | IC2 CFGR2    |  .....    |

*/
uint8_t rx_cfg[TOTAL_IC][7];
/*!<
  the rx_cfg[][8] array stores the data that is read back from a LTC6803-1 daisy chain.
  The configuration data for each IC  is stored in blocks of 7 bytes. Below is an table illustrating the array organization:

|rx_config[0][0]|rx_config[0][1]|rx_config[0][2]|rx_config[0][3]|rx_config[0][4]|rx_config[0][5]|rx_config[0][6]  |rx_config[1][0]|rx_config[1][1]|  .....    |
|---------------|---------------|---------------|---------------|---------------|---------------|-----------------|---------------|---------------|----------|
|IC1 CFGR0      |IC1 CFGR1      |IC1 CFGR2      |IC1 CFGR3      |IC1 CFGR4      |IC1 CFGR5      |IC1 PEC          |IC2 CFGR0      |IC2 CFGR1      |  .....    |
*/

/**
 * given a config array sized [TOTAL_IC]x[6], 
 *  sets the value of the masked vars to match the corrosponding bits in 'val' 
 *  while retaining the non masked values
 */
void setCFG(uint8_t  cfg[][6], uint8_t reg, uint8_t mask, uint8_t val);

/**
 * forces standby mode. soft
 * 
 *    if ADC measurements in progress, can potentially cause returns to
 *      be of a indeterminate state.
 * 
 * - settings according to pg13, pg14
 * - does not effect SPI inputs
 * - is not the most power effecient it can be
 */
void toModeStandby();

void setup() {
  //-----------------------------------------------------------------------------
  // init
  //-----------------------------------------------------------------------------

  //serial for the ltc6803
  //  also sets Linduino to have a 1MHz clock
  LTC6803_initialize();

  //serial for the ui
  Serial.begin(115200);

  //-----------------------------------------------------------------------------
  // setup
  //-----------------------------------------------------------------------------

  //turn on watch dog timers
  setCFG(tx_cfg, 0, (1<<7), 0xFF);

  // to measure mode

  // set 500ms comparator period, no powerdown
  //  -pg 24
  setCFG(tx_cfg, 0, 0x7, 4);
}

void loop() {
  //read ic temp
  // if(LTC6803_rdtmp(TOTAL_IC, temp_codes)){
  //   //womp womp
  // }

  // start cell voltage adc conversions non distruptive to discharge
  //    and polls status
  //  -pg 22, 18
  spi_write_array(2, LTC6803_Cmd::STOWDC_ALL.arr);
  delay(15); // wait for adcs to complete

  //reads cell voltage (mV)
  if(LTC6803_rdcv(TOTAL_IC, cell_codes)){
    //uh oh
  } else {
    //do stuff
    // decide what gets discharged or what ever    


  }

  //clear adc registers, distruptive to discharge
  spi_write_array(2, LTC6803_Cmd::STCVAD_Clear.arr);

  //test, clock ladder the gpios to see if changes are actually being applied
  {
   static uint8_t ladder = 0;
    ladder++;
    setCFG(tx_cfg, 0, 0x60, ladder << 5);

    // write changes
    LTC6803_wrcfg(TOTAL_IC, tx_cfg);
  }

}



void toModeStandby(){
  // set the compariator duty cycle to 0, thats all there is
  setCFG(tx_cfg, 0, 0x7, 0);

  // write changes
  LTC6803_wrcfg(TOTAL_IC, tx_cfg);
}

void setCFG(uint8_t cfg[][6], uint8_t reg, uint8_t mask, uint8_t val)
{
  for(uint8_t i = 0; i < TOTAL_IC; i++){
    cfg[i][reg] = (cfg[i][reg] & (~mask)) | (val & mask);
  }  
}