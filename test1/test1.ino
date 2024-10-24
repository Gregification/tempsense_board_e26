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

#define CS 4

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

void print_config();
void print_rxconfig();
void serial_print_hex(uint8_t data);
void print_temp();

void setup() {
  //-----------------------------------------------------------------------------
  // init
  //-----------------------------------------------------------------------------
  pinMode(CS, OUTPUT);

  Serial.begin(9600);

  SPI.begin();  
  SPI.beginTransaction(SPISettings(100, MSBFIRST, SPI_MODE0));

  //-----------------------------------------------------------------------------
  // setup
  //-----------------------------------------------------------------------------

  //turn on watch dog timers
  //setCFG(tx_cfg, 0, (1<<7), 0xFF);

  // init config array
  //    - enable watch dog timer
  //    - enable level polling
  //    -

  // to measure mode

  // set 500ms comparator period, no powerdown
  //  -pg 24
  // setCFG(tx_cfg, 0, 0x7, 4);

}

void loop() {
  // start cell voltage adc conversions non distruptive to discharge
  //    and polls status
  //  -pg 22, 18
  spi_write_array(2, LTC6803_Cmd::STOWDC_ALL.arr);
  delay(10); // wait for adcs to complete

  static int c = 0, a = 0;
  setCFG(tx_cfg, 0, 0xFF, c);
  setCFG(tx_cfg, 1, 0xFF, c);
  setCFG(tx_cfg, 2, 0xFF, c);
  setCFG(tx_cfg, 3, 0xFF, c);
  setCFG(tx_cfg, 4, 0xFF, c);
  setCFG(tx_cfg, 5, 0xFF, c);

  LTC6803_wrcfg(TOTAL_IC, tx_cfg);
  
  Serial.print("sent:");
  print_config();

  Serial.print("read:");
  int error = 0; 
  if(error = LTC6803_rdcfg(TOTAL_IC, rx_cfg)){
    Serial.print("failed read, code : ");
    Serial.println(error);
    print_rxconfig();
  }
  else {
    c++;
    print_rxconfig();
    Serial.print("yippie");
    Serial.println(a);
    delay(500);
    a = 0;
  }
  a++;
  delay(100);
  Serial.println("------------------------------------------------");

}

void setCFG(uint8_t cfg[][6], uint8_t reg, uint8_t mask, uint8_t val)
{
  for(uint8_t i = 0; i < TOTAL_IC; i++){
    cfg[i][reg] = (cfg[i][reg] & (~mask)) | (val & mask);
  }  
}

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
    serial_print_hex((uint8_t)(cfg_pec));
    Serial.println();
  }
  Serial.println();
}

void print_rxconfig()
{
  Serial.println("Received Configuration ");
  for (int current_ic=0; current_ic<TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    Serial.print(": 0x");
    serial_print_hex(rx_cfg[current_ic][0]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][1]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][2]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][3]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][4]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][5]);
    Serial.print(", Received PEC: 0x");
    serial_print_hex(rx_cfg[current_ic][6]);

    Serial.println();
  }
  Serial.println();
}

void serial_print_hex(uint8_t data)
{
  if (data < 16)
  {
    Serial.print("0");
    Serial.print((byte)data,HEX);
  }
  else
    Serial.print((byte)data,HEX);
}

void print_temp()
{

  for (int current_ic =0 ; current_ic < TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    for (int i=0; i < 2; i++)
    {
      Serial.print(" Temp-");
      Serial.print(i+1,DEC);
      Serial.print(":");
      Serial.print(temp_codes[current_ic][i]*0.0015,4);
      Serial.print(",");
    }
    Serial.print(" ITemp");
    Serial.print(":");
    Serial.print((temp_codes[current_ic][2]*0.1875)-274.15,4);
    Serial.println();
  }
  Serial.println();
}