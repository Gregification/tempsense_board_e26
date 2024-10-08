/**
 * minimal spi test
 *  sends the read configuration command thouhg spi, attempts to read back anyhting
 * 
 * minimal version of the demo code
 * 
 * LTC6803-1 datasheet used : https://www.analog.com/media/en/technical-documentation/data-sheets/680313fa.pdf
*/

// imports from the "DC1651A.ino" file
#include <Arduino.h>
#include <stdint.h>
#include "LTC68031.h"
#include <SPI.h>
#include "LTC6803_cmds.h"

const uint8_t TOTAL_IC = 1;

uint8_t tx_cfg[TOTAL_IC][6];
uint8_t rx_cfg[TOTAL_IC][7];

/**
 * given a config array sized [TOTAL_IC]x[6], 
 *  sets the value of the masked vars to match the corrosponding bits in 'val' 
 *  while retaining the non masked values
 */
void setCFG(uint8_t  cfg[][6], uint8_t reg, uint8_t mask, uint8_t val);

void print_config();

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


}

void loop() {
  // read cfg
  LTC6803_rdcfg(TOTAL_IC, rx_cfg);

  // writes cfg
  // test, clock ladder the gpios to see if changes are actually being applied
  {
   static uint8_t ladder = 0;
    ladder++;
    setCFG(tx_cfg, 0, 0x60, ladder << 5);
    
    LTC6803_wrcfg(TOTAL_IC, tx_cfg);

    delay(1000);
  }

}

void setCFG(uint8_t cfg[][6], uint8_t reg, uint8_t mask, uint8_t val)
{
  for(uint8_t i = 0; i < TOTAL_IC; i++){
    cfg[i][reg] = (cfg[i][reg] & (~mask)) | (val & mask);
  }  
}

//!Prints the configuration data that is going to be written to the LTC6803 to the serial port.
void print_config()
{
  int cfg_pec;

  Serial.println("Written Configuration: ");
  for (int current_ic = 0; current_ic<TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    Serial.print(": ");
    Serial.print("0x");
    serial_print_hex(tx_cfg[current_ic][0]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][1]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][2]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][3]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][4]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][5]);
    Serial.print(", Calculated PEC: 0x");
    cfg_pec = pec8_calc(6,&tx_cfg[current_ic][0]);
    serial_print_hex((uint8_t)(cfg_pec>>8));
    Serial.print(", 0x");
    serial_print_hex((uint8_t)(cfg_pec));
    Serial.println();
  }
  Serial.println();
}
