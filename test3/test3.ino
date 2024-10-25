/** 
 * see the main README on github for the data sheets
 *    https://github.com/Gregification/tempsense_board_e26
 * 
 * what this does:
 *    - periodicly polls the LTC for cell voltages, forwards that and related info to BMS.
 *    - adc conversions are non-distruptive of discharge
 *    - LTC preforms a internal discharge during measurements
 *    - because of internal discharge, there is logic to limit time discharging
 * 
 * what this does not do:
 *    - configure for # of operating cells
*/

// imports from the "DC1651A.ino" file
#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"           // "hardware definitions for the Linduino"
#include "LT_SPI.h"             // "Routines to communicate with ATmega328P's hardware SPI port."
#include "UserInterface.h"
#include "LTC68031.h"
#include <SPI.h>

#include "LTC6803_support.h"

void print_config();
void print_rxconfig();
void serial_print_hex(uint8_t data);
void print_temp();
void print_cells();

void setup() {
  //-----------------------------------------------------------------------------
  // init
  //-----------------------------------------------------------------------------

  // slave select pin
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, LOW);

  Serial.begin(9600);

  SPI.begin();  
  SPI.beginTransaction(SPISettings(LTC6803_SPI_CLK_SPEED, MSBFIRST, SPI_MODE0));

  //-----------------------------------------------------------------------------
  // setup
  //-----------------------------------------------------------------------------

  /**
   * index  : value : purpose of editing it
   *  0,1,2 : 4     : set IC internal poll period to 130ms, no powerdowns on Vref.
   *  3     : 0     : 12 cell
   *  4     : 1     : enable level polling (because the alternative, toggle polling, sucks).
   *  5,6   : 1     : disiable GPIO 2 & 1
   *  7     : ~     : no effect
   * 
   * notes:
   *    - not using interrupts because were spam polling this anyways. and theres some
   *      extra things to consider when using interrupts like masking disconnected 
   *      cells, the act of actually catching a interrupt over a noisy line, etc.
  */
  setCFG(tx_cfg, -1, 0, 0xFF  , 0b1111'1000 | CDC);

  // "mask" the input chanlles of unused cells, doc says this is required
  //    unknown consequences if this isnt done (probably effects just the interrupt 
  //    system we dont use anyways)
  mask_unused_cells(tx_cfg);

}

void loop() {
  // for period timing
  unsigned long loop_start = millis();

  // start cell discharge

  // start cell voltage adc measurements, non distruptive to discharge
  spi_tx_command(LTC6803_CMD_STOWAD_ALL);
  
  // wait for it to complete
  delay(CELL_V_MEASURE_TIME_mS);

  // stop cell discharge
  static int c = 0, a = 0, b;
  setCFG(tx_cfg, 0, 0xFF, 0xF1);
  LTC6803_wrcfg(TOTAL_IC, tx_cfg);
  
  int error = 0; 
  if(error = LTC6803_rdcfg(TOTAL_IC, rx_cfg)){
    
  }
  else {
    c++;
    print_rxconfig();
    Serial.print("yippie ");
    Serial.print(a);
    Serial.print(" ");
    Serial.print(b);
    Serial.print("  ");
    Serial.println(a == b);

    LTC6803_stcvad();
    print_cells();
      //reads cell voltage (mV)
    if(LTC6803_rdcv(TOTAL_IC, cell_codes)){
      //uh oh
      Serial.println("failed cell v read");
    } else {
      //do stuff
      // decide what gets discharged or what ever

    }

    //delay(500);
    b = a;
    a = 0;
  }
  a++;
  //delay(100);
 // Serial.println("------------------------------------------------");
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
    serial_print_hex((uint8_t)(cfg_pec>>8));
    Serial.print(", 0x");
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

void print_cells()
{
  for (int current_ic = 0 ; current_ic < TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    for (int i=0; i<12; i++)
    {
      Serial.print(" C");
      Serial.print(i+1,DEC);
      Serial.print(":");
      Serial.print(cell_codes[current_ic][i]*0.0015,4);
      Serial.print(",");
    }
    Serial.println();
  }
  Serial.println();
}
