/**
 * basic management of adafrut file system. only navigates root directory
 *    - list files
 *    - delete file
 *    - dump file contents to serial (recommended to capture with Putty)
 * 
 */

#define SERIAL_BAUD     9600

#include <stdint.h>     
#include <array>        
#include <string>       // to_string

#include <Arduino.h>
#include <Adafruit_SPIFlash.h>  // fs
#include <SdFat.h>              // fs

#include "flash_config.h"                   // fs
Adafruit_SPIFlash flash(&flashTransport);   // fs
FatVolume fatfs;                            // fs

enum CMDS : int {
    HELP = 0,
    INFO,
    LIST,
    DUMP,
    DELETE
};

String read_str(){
  while(Serial.available() == 0)
    yield();
  String str = Serial.readStringUntil('\n');
  Serial.println(str);
  return str;
}

void setup(){
    Serial.begin(SERIAL_BAUD);
    Serial.println("filedumper");

    if(!flash.begin()){
      Serial.println("setup: init: failed to init flash chip");
      while (1) {
        yield();
      }
    }

    if (!fatfs.begin(&flash)) {
      Serial.println("Error, failed to mount newly formatted filesystem!");
      Serial.println("Was the flash chip formatted with the fatfs_format example? (is a built in arduino example)");

      while (1) {
        yield();
      }
    }
    Serial.println("Mounted filesystem!");

    Serial.print(">");
}

void loop(){
    if(Serial.available() == 0)
      return;

    int cmd = Serial.readStringUntil('\n')[0] - '0';
    
    Serial.print(cmd);
    Serial.print("-----------enter ");
    Serial.print(CMDS::HELP);
    Serial.print(" for help");
    Serial.println("------------");

    switch(cmd){
        default:
        case CMDS::HELP:{
                Serial.print(CMDS::HELP);
                Serial.println(" HELP");
                Serial.print(CMDS::INFO);
                Serial.println(" INFO");
                Serial.print(CMDS::LIST);
                Serial.println(" LIST");
                Serial.print(CMDS::DUMP);
                Serial.println(" DUMP");
                Serial.print(CMDS::DELETE);
                Serial.println(" DELETE");
            }break;

        case CMDS::INFO :{
                Serial.println("info");
                
                Serial.print("JEDEC ID: 0x");
                Serial.println(flash.getJEDECID(), HEX);
                Serial.print("Flash size: ");
                Serial.print(flash.size() / 1024);
                Serial.println(" KB");
            }break;

        case CMDS::LIST :{
                Serial.println("list");
                fatfs.ls(LS_DATE | LS_A | LS_R | LS_SIZE);
            }break;

        case CMDS::DELETE :{
                Serial.print("delete,  file : ");
                String file_pth = read_str();

                if(fatfs.exists(file_pth)){
                  fatfs.remove(file_pth);
                } else {
                  Serial.println("file does not exist");
                }
            }break;

        case CMDS::DUMP :{
                Serial.print("dump,  file : ");
                String file_pth = read_str();

                if(fatfs.exists(file_pth)){
                  File32 file = fatfs.open(file_pth);
                  while(file.available()){
                    char c = file.read();
                    Serial.print(c);
                  }
                  file.close();
                } else {
                  Serial.println("file does not exist");
                }
            }break;
    }

    Serial.print(">");
}
