/** 
 * see the main README on github for the data sheets
 *    https://github.com/Gregification/tempsense_board_e26
 * 
 * what this does:
 *    - periodicly polls the LTC for cell voltages, forwards that and related info to BMS.
 *    - adc conversions are non-distruptive of discharge
 *    - LTC preforms a internal discharge during measurements
 * 
 * what this does not do:
 *    - configure for # of operating cells
 *    - care about OV/UV interrupts
 * 
 * details:
 *    - all ic's will discharge the same cell numbers at the same time
 *    - voltage measurements are done in 4 cycles.
 *        this is to prevent over currenting the ic, a 2 cycle system is sugested by the 
 *        datasheet(P45B.4) but that would mean that each cell only gets 1 command sent to stop
 *        discharging, if that command is missed then the next cycle will have 2 cells 
 *        discharging in series, bad! by having 4 cycles a cell must miss at least 2 stop 
 *        commands in a row before it risks over currenting the ic.
*/

// imports from the "DC1651A.ino" file
#include <Arduino.h>
#include <stdint.h>
#include <SdFat.h>
#include <SPI.h>

#include "Linduino.h"           // "hardware definitions for the Linduino"
#include "LT_SPI.h"             // "Routines to communicate with ATmega328P's hardware SPI port."
#include "UserInterface.h"
#include "LTC68031.h"

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

  // file system
  if(!flash.begin()){
    Serial.println("setup: init: failed to init flash chip");
    while(1)
      yield();
  }

  // create file & headers
  {
    File32 log = fatfs.open(LOG_FILE_NAME, FILE_WRITE);
    if(log){
      log.prinln("Time(ms), C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12");
      log.close();
    } else {
      Serial.println("unable to create log file");
      log.close();
      while(1) 
        yield();
    }
  }

  //-----------------------------------------------------------------------------
  // POST
  //-----------------------------------------------------------------------------



  //-----------------------------------------------------------------------------
  // apply enviroment settings
  //-----------------------------------------------------------------------------
  // "-" : stack level dependent value
  // "~" : value does not matter, ignored
  // "[]": treat range as digit

  /** register 0
   * index  : value : purpose of editing it
   * [2,1,0]: 1     : disable OV/UV comparators but keep ADC on
   *  3     : -     : 10/12 cell mode
   *  4     : 1     : enable level polling (because the alternative, toggle polling, sucks)
   *  5-6   : 1     : turn off GPIO 2 & 1
   *  7     : ~     : no effect
  */
  setCFG(tx_cfg, -1, 0, 0xFF, 0xF1);

  /** register 1
   * index  : value : purpose of editing it
   *  0-7   : 0     : ensure discharge disabled, cells 1-8
   */
  setCFG(tx_cfg, -1, 1, 0xFF, 0);

  /** register 2
   * index  : value : purpose of editing it
   *  0-3   : 0     : ensure discharge disabled, cells 9-12
   *  4-7   : 1     : disable OV/UV interrupts, cells 1-4
   */
  setCFG(tx_cfg, -1, 2, 0xFF, 0xF0);

  /** register 3
   * index  : value : purpose of editing it
   *  0-7   : 1     : disable OV/UV interrupts, cells 5-12
   */
  setCFG(tx_cfg, -1, 2, 0xFF, 0xFF);

  // apply stack level speficic settings
  #ifdef CELL10_MODE_IF_POSSIBLE // the only stack specific setting
    
    for(uint8_t i = 0; i < TOTAL_IC; i++){    
        // if using 10 or less cells
        if(used_cells[i] <= 10){
          //enable 10 cell mode
          setCFG(tx_cfg, i, 0, 1<<3, 0xFF);
        }
    }

  #endif
  
  //-----------------------------------------------------------------------------
  // write config
  //  - all this fluff is to ensure the config was actually written
  //      i bother to ensure this because this is the only part of the program 
  //      that wont self correct
  //-----------------------------------------------------------------------------

  // - keep trying to write untill the received config is the same as the transmitted
  while(true){
    LTC6803_wrcfg(TOTAL_IC, tx_cfg);
    LTC6803_rdcfg(TOTAL_IC, rx_cfg);

    // the only registers that indicate our changes are 0,1,2,3
    //    we ignore the 7th bit of reg 0 since thats the watchdog timer which is irrevlant

    uint8_t error = 0;

    for(uint8_t i = 0; i < TOTAL_IC; i++){
      // reg 0
      if(memcmp(tx_cfg[i][0], rx_cfg[i][0], 6)){
        error |= 1 << 7;
        error |= i;
        break;
      }

      // reg 1 - 3
      for(uint8_t j = 1; j < 4; j++){
        if(memcmp(tx_cfg[i][j], rx_cfg[i][j], 7)){
          error |= i;
          break;
        }
      }
    } 

    if(error == 0){
      break;
    } else {
      Serial.print("setup ");
      Serial.print(__LINE__);
      Serial.print(": error writing config, trying again. error 0x");
      Serial.println(error, HEX);

      delay(200);
    }

  }

  Serial.print("setup complete :) . time taken (mS):");
  Serial.println(millis());
}

void loop() {
  // for period timing
  static unsigned long loop_timer = 0;

  //-----------------------------------------------------------------------------
  // discharge & measure cells
  //-----------------------------------------------------------------------------

  #ifdef DISCHARGE_WHEN_ADC
  // discharge cells in pairs of 3

  for(uint8_t discharge_cycle = 0; discharge_cycle < 4; discharge_cycle++){

    // cells to be discharged, all other cells are to stop discharging
    setCFG(tx_cfg, -1, 1, 0xFF, (1 << discharge_cycle) | (0x10 << discharge_cycle));
    setCFG(tx_cfg, -1, 2, 0x0F, (1 << discharge_cycle);
    LTC6803_wrcfg(TOTAL_IC, tx_cfg);

    // start cell voltage adc measurements, non distruptive to discharge
    // target only the cells being discharged
    // id rather not make a giant switch statement so heres this indexing thing instead
    static uint8_t cell_discharge_cmd_pec_codes[] = {
      LTC6803_CMD_STCVDC_Cell1.cmd.pec,
      LTC6803_CMD_STCVDC_Cell2.cmd.pec,
      LTC6803_CMD_STCVDC_Cell3.cmd.pec,
      LTC6803_CMD_STCVDC_Cell4.cmd.pec,
      LTC6803_CMD_STCVDC_Cell5.cmd.pec,
      LTC6803_CMD_STCVDC_Cell6.cmd.pec,
      LTC6803_CMD_STCVDC_Cell7.cmd.pec,
      LTC6803_CMD_STCVDC_Cell8.cmd.pec,
      LTC6803_CMD_STCVDC_Cell9.cmd.pec,
      LTC6803_CMD_STCVDC_Cell10.cmd.pec,
      LTC6803_CMD_STCVDC_Cell11.cmd.pec,
      LTC6803_CMD_STCVDC_Cell12.cmd.pec      
    };
    spi_tx(2, {0x60 + discharge_cycle+1, cell_discharge_cmd_pec_codes[discharge_cycle+1]});
    spi_tx(2, {0x60 + discharge_cycle+5, cell_discharge_cmd_pec_codes[discharge_cycle+5]});
    spi_tx(2, {0x60 + discharge_cycle+9, cell_discharge_cmd_pec_codes[discharge_cycle+9]});

    // wait for adc's to complete
    delay(13); // ltc.24    
  }

  // stop all cell discharge
  setCFG(tx_cfg, -1, 1, 0xFF, 0);
  setCFG(tx_cfg, -1, 2, 0x0F, 0);
  LTC6803_wrcfg(TOTAL_IC, tx_cfg);

  #else
  // measure all cells
  spi_tx_command(LTC6803_Cmd_STCVDC_ALL);

  // wait for adc's to complete
  delay(13); // ltc.24

  #endif
  
  //-----------------------------------------------------------------------------
  // read & process adc results
  //-----------------------------------------------------------------------------

  if(LTC6803_rdcv(TOTAL_IC, cell_codes)){
    // pec error
    Serial.println("failed cell v read");
  } else {
    // can bus code also goes here, somewhere

    // write to file
    // using a new file handler every time just to test if it works, im guessing 
    //    theres around a 200ms time between logging. seems reasonable?
    File32 fh = ffs.open(LOG_FILE_NAME, FILE_WRITE);

    fh.println(millis());

  }

  // clear voltage registers so we can tell if each adc is running or not by if
  // the respective register is zero
  spi_tx_command(LTC6803_CMD_STCVAD_Clear);

  //-----------------------------------------------------------------------------
  // loop timing
  //-----------------------------------------------------------------------------

  unsigned long current_time = millis();
  
  if((current_time - loop_timer) < MEASUREMENT_PERIOD_mS){
    current_time = millis();
    yield();
    // delay(MEASUREMENT_PERIOD_mS - (current_time - loop_timer));
  }

  loop_timer = current_time;

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
