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

#include <stdint.h>
#include <array>

// imports from the "DC1651A.ino" file
#include <Arduino.h>
#include <Adafruit_SPIFlash.h>  // fs
#include <SdFat.h>              // fs
#include <SPI.h>

#include "Linduino.h"           // "hardware definitions for the Linduino"
#include "LT_SPI.h"             // "Routines to communicate with ATmega328P's hardware SPI port."
#include "UserInterface.h"
#include "LTC68031.h"

#include "LTC6803_support.h"

#include "flash_config.h"                   // fs
Adafruit_SPIFlash flash(&flashTransport);   // fs
FatVolume fatfs;                            // fs

void print_config();
void print_rxconfig();
void serial_print_hex(uint8_t data);
void print_temp();
void print_cells();

void slow_blink(uint8_t);
void fast_blink(uint8_t);
void end_blink(){ delay(1500);}

void dump_logs_to_serial();
void delete_logs();

void setup() {
  //-----------------------------------------------------------------------------
  // init
  //-----------------------------------------------------------------------------

  // slave select pin
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, LOW);

  // not used for now
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  Serial.begin(SERIAL_BAUD);

  SPI.begin();  
  SPI.beginTransaction(SPISettings(LTC6803_SPI_CLK_SPEED, MSBFIRST, SPI_MODE0));

  // file system
  if(!flash.begin()){
    Serial.println("setup: init: failed to init flash chip");
  }

  //-----------------------------------------------------------------------------
  // POST
  //-----------------------------------------------------------------------------

  slow_blink(2);
  end_blink();

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
    yield();
    
    LTC6803_wrcfg(TOTAL_IC, tx_cfg);
    LTC6803_rdcfg(TOTAL_IC, rx_cfg);

    // the only registers that indicate our changes are 0,1,2,3
    //    we ignore the 7th bit of reg 0 since thats the watchdog timer which is irrevlant

    uint8_t error = 0;

    for(uint8_t i = 0; i < TOTAL_IC; i++){
      // reg 0
      //   we ignore WDT and the GPIO
      if((tx_cfg[i][0] & 0x1F) != (rx_cfg[i][0] & 0x1F)){
        error |= 1 << 7;
        error |= i;
        break;
      }

      // reg 1 - 3
      for(uint8_t j = 1; j < 4; j++){
        if(tx_cfg[i][j] != rx_cfg[i][j]){
          error |= i;
          break;
        }
      }
    } 

    if(error == 0){
      break;
    } else {
      Serial.print(__LINE__);
      Serial.print(" error writing config, trying again. error 0x");
      Serial.println(error, HEX);
    }

    slow_blink(1);
    end_blink();
  }

  Serial.print(__LINE__);
  Serial.print("setup complete :) . time taken (mS):");
  Serial.println(millis());
}

void loop() {

  if(digitalRead(LOG_DUMP_PIN) == HIGH)
    dump_logs_to_serial();
  else if(digitalRead(LOG_DELETE_PIN) == HIGH) {
    slow_blink(5);
    end_blink();

    static uint8_t toggler;
    if(toggler & 1 == 0){
      delete_logs();
    }

    toggler++;
  }

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
    setCFG(tx_cfg, -1, 2, 0x0F, (1 << discharge_cycle));
    LTC6803_wrcfg(TOTAL_IC, tx_cfg);

    // start cell voltage adc measurements, non distruptive to discharge
    // target only the cells being discharged
    switch(discharge_cycle){
      case 0:
        // dis cells 1,5,9
        spi_tx_command(LTC6803_CMD_STCVDC_Cell1);
        spi_tx_command(LTC6803_CMD_STCVDC_Cell5);
        spi_tx_command(LTC6803_CMD_STCVDC_Cell9);
        break;
      case 1:
        // dis cells 2,6,10
        spi_tx_command(LTC6803_CMD_STCVDC_Cell2);
        spi_tx_command(LTC6803_CMD_STCVDC_Cell6);
        spi_tx_command(LTC6803_CMD_STCVDC_Cell10);
        break;
      case 2:
        // dis cells 3,7,11
        spi_tx_command(LTC6803_CMD_STCVDC_Cell3);
        spi_tx_command(LTC6803_CMD_STCVDC_Cell7);
        spi_tx_command(LTC6803_CMD_STCVDC_Cell11);
        break;
      case 3:
        // dis cells 4,8,12
        spi_tx_command(LTC6803_CMD_STCVDC_Cell4);
        spi_tx_command(LTC6803_CMD_STCVDC_Cell8);
        spi_tx_command(LTC6803_CMD_STCVDC_Cell12);
        break;
    }

    // wait for adc's to complete
    delay(13); // ltc.24    
  }

  // stop all cell discharge
  setCFG(tx_cfg, -1, 1, 0xFF, 0);
  setCFG(tx_cfg, -1, 2, 0x0F, 0);
  LTC6803_wrcfg(TOTAL_IC, tx_cfg);

  #else
  // measure all cells
  spi_tx_command(LTC6803_CMD_STCVDC_ALL);

  // wait for adc's to complete
  delay(13); // ltc.24

  #endif
  
  //-----------------------------------------------------------------------------
  // read & process adc results
  //-----------------------------------------------------------------------------

  if(LTC6803_rdcv(TOTAL_IC, cell_codes)){
    // pec error
    Serial.print(__LINE__);
    Serial.println("failed cell v read");

    fast_blink(2);
    end_blink();
  } else {
    // can bus code also goes here, somewhere

    // write to file
    // using a new file handler every time just to test if it works, im guessing 
    //    theres around a 200ms time between logging. seems reasonable?
    {
      File32 log = fatfs.open(LOG_FILE_NAME, FILE_WRITE);
      if(log){
        // format
        //    [time in ms],[C1],[C2],[C3],[C4],[C5],[C6],[C7],[C8],[C9],[C10],[C11],[C12],[C1 of IC2 ...]
        // everything is in hex
        
        log.print(loop_timer, HEX);

        for(uint8_t ic = 0; ic < TOTAL_IC; ic++)
          for(uint8_t s = 0; s < 12; s++){
            log.print(",");
            log.print(adc_CV_to_mV(cell_codes[ic][s]), HEX);
          }

        log.println();

        log.close();
      } else {
        Serial.print(__LINE__);
        Serial.println(" unable to write log file");
        
        fast_blink(1);
        end_blink();
      }
    }

  }

  // clear voltage registers so we can tell if each adc is running or not by if
  // the respective register is zero
  spi_tx_command(LTC6803_CMD_STCVAD_Clear);

  //-----------------------------------------------------------------------------
  // loop timing
  //-----------------------------------------------------------------------------

  unsigned long current_time = millis();
  
  if((current_time - loop_timer) < MEASUREMENT_PERIOD_mS){
    yield();
    current_time = millis();
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

void slow_blink(uint8_t count){
  for(;count > 0; count--){
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(800);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(800);
  }
}

void fast_blink(uint8_t count){
  for(;count > 0; count--){
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(300);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(300);
  }
}

void dump_logs_to_serial(){

}

void delete_logs(){

}
