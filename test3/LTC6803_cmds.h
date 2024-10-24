/**
 * commands for the ltc6803
 * 
 * - ltc.21
 * - oddly enough, the linduino coded hardcode the values. great legibility guys
 * - using a bunch of macros because its a industry standard
 */

#ifndef __LTC6803_CMDS_H
#define __LTC6803_CMDS_H

union LTC6803_Cmd {
    struct Cmd_s{
        uint8_t code;       // command code
        uint8_t pec;        // packet error code
    } cmd;
    // static_assert(std::is_standard_layout<Cmd_s>::value);

    uint8_t arr[2];
};    

    //  command name                                  // CMD   PEC  
#define LTC6803_CMD_WRCFG                   LTC6803_Cmd{ 0x01, 0xC7}
#define LTC6803_CMD_RDCFG                   LTC6803_Cmd{ 0x02, 0xDE}
#define LTC6803_CMD_RDCV                    LTC6803_Cmd{ 0x04, 0xDC}
#define LTC6803_CMD_RDCVA                   LTC6803_Cmd{ 0x06, 0xD2}
#define LTC6803_CMD_RDCVB                   LTC6803_Cmd{ 0x08, 0xF8}
#define LTC6803_CMD_RDCVC                   LTC6803_Cmd{ 0x0A, 0xF6}
#define LTC6803_CMD_RDFLG                   LTC6803_Cmd{ 0x0C, 0xE4}
#define LTC6803_CMD_RDTMP                   LTC6803_Cmd{ 0x0E, 0xEA}
#define LTC6803_CMD_STCVAD_ALL              LTC6803_Cmd{ 0x10, 0xB0}
#define LTC6803_CMD_STCVAD_Cell1            LTC6803_Cmd{ 0x11, 0xB7}
#define LTC6803_CMD_STCVAD_Cell2            LTC6803_Cmd{ 0x12, 0xBE}
#define LTC6803_CMD_STCVAD_Cell3            LTC6803_Cmd{ 0x13, 0xB9}
#define LTC6803_CMD_STCVAD_Cell4            LTC6803_Cmd{ 0x14, 0xAC}
#define LTC6803_CMD_STCVAD_Cell5            LTC6803_Cmd{ 0x15, 0xAB}
#define LTC6803_CMD_STCVAD_Cell6            LTC6803_Cmd{ 0x16, 0xA2}
#define LTC6803_CMD_STCVAD_Cell7            LTC6803_Cmd{ 0x17, 0xA5}
#define LTC6803_CMD_STCVAD_Cell8            LTC6803_Cmd{ 0x18, 0x88}
#define LTC6803_CMD_STCVAD_Cell9            LTC6803_Cmd{ 0x19, 0x8F}
#define LTC6803_CMD_STCVAD_Cell10           LTC6803_Cmd{ 0x1A, 0x86}
#define LTC6803_CMD_STCVAD_Cell11           LTC6803_Cmd{ 0x1B, 0x81}
#define LTC6803_CMD_STCVAD_Cell12           LTC6803_Cmd{ 0x1C, 0x94}
#define LTC6803_CMD_STCVAD_Clear            LTC6803_Cmd{ 0x1D, 0x93}
#define LTC6803_CMD_STCVAD_SelfTest1        LTC6803_Cmd{ 0x1E, 0x9A}
#define LTC6803_CMD_STCVAD_SelfTest2        LTC6803_Cmd{ 0x1F, 0x9D}
#define LTC6803_CMD_STOWAD_ALL              LTC6803_Cmd{ 0x20, 0x20}
#define LTC6803_CMD_STOWAD_Cell1            LTC6803_Cmd{ 0x21, 0x27}
#define LTC6803_CMD_STOWAD_Cell2            LTC6803_Cmd{ 0x22, 0x2E}
#define LTC6803_CMD_STOWAD_Cell3            LTC6803_Cmd{ 0x23, 0x29}
#define LTC6803_CMD_STOWAD_Cell4            LTC6803_Cmd{ 0x24, 0x3C}
#define LTC6803_CMD_STOWAD_Cell5            LTC6803_Cmd{ 0x25, 0x3B}
#define LTC6803_CMD_STOWAD_Cell6            LTC6803_Cmd{ 0x26, 0x32}
#define LTC6803_CMD_STOWAD_Cell7            LTC6803_Cmd{ 0x27, 0x35}
#define LTC6803_CMD_STOWAD_Cell8            LTC6803_Cmd{ 0x28, 0x18}
#define LTC6803_CMD_STOWAD_Cell9            LTC6803_Cmd{ 0x29, 0x1F}
#define LTC6803_CMD_STOWAD_Cell10           LTC6803_Cmd{ 0x2A, 0x16}
#define LTC6803_CMD_STOWAD_Cell11           LTC6803_Cmd{ 0x2B, 0x11}
#define LTC6803_CMD_STOWAD_Cell12           LTC6803_Cmd{ 0x2C, 0x04}
#define LTC6803_CMD_STTMPAD_ALL             LTC6803_Cmd{ 0x30, 0x50}
#define LTC6803_CMD_STTMPAD_External1       LTC6803_Cmd{ 0x31, 0x57}
#define LTC6803_CMD_STTMPAD_External2       LTC6803_Cmd{ 0x32, 0x5E}
#define LTC6803_CMD_STTMPAD_Internal        LTC6803_Cmd{ 0x33, 0x59}
#define LTC6803_CMD_STTMPAD_SelfTest1       LTC6803_Cmd{ 0x3E, 0x7A}
#define LTC6803_CMD_STTMPAD_SelfTest2       LTC6803_Cmd{ 0x3F, 0x7D}
#define LTC6803_CMD_PLADC                   LTC6803_Cmd{ 0x40, 0x07}
#define LTC6803_CMD_PLINT                   LTC6803_Cmd{ 0x50, 0x77}
#define LTC6803_CMD_DAGN                    LTC6803_Cmd{ 0x52, 0x79}
#define LTC6803_CMD_RDDGNR                  LTC6803_Cmd{ 0x54, 0x6B}
#define LTC6803_CMD_STCVDC_ALL              LTC6803_Cmd{ 0x60, 0xE7}
#define LTC6803_CMD_STCVDC_Cell1            LTC6803_Cmd{ 0x61, 0xE0}
#define LTC6803_CMD_STCVDC_Cell2            LTC6803_Cmd{ 0x62, 0xE9}
#define LTC6803_CMD_STCVDC_Cell3            LTC6803_Cmd{ 0x63, 0xEE}
#define LTC6803_CMD_STCVDC_Cell4            LTC6803_Cmd{ 0x64, 0xFB}
#define LTC6803_CMD_STCVDC_Cell5            LTC6803_Cmd{ 0x65, 0xFC}
#define LTC6803_CMD_STCVDC_Cell6            LTC6803_Cmd{ 0x66, 0xF5}
#define LTC6803_CMD_STCVDC_Cell7            LTC6803_Cmd{ 0x67, 0xF2}
#define LTC6803_CMD_STCVDC_Cell8            LTC6803_Cmd{ 0x68, 0xDF}
#define LTC6803_CMD_STCVDC_Cell9            LTC6803_Cmd{ 0x69, 0xD8}
#define LTC6803_CMD_STCVDC_Cell10           LTC6803_Cmd{ 0x6A, 0xD1}
#define LTC6803_CMD_STCVDC_Cell11           LTC6803_Cmd{ 0x6B, 0xD6}
#define LTC6803_CMD_STCVDC_Cell12           LTC6803_Cmd{ 0x6C, 0xC3}
#define LTC6803_CMD_STOWDC_ALL              LTC6803_Cmd{ 0x70, 0x97}
#define LTC6803_CMD_STOWDC_Cell1            LTC6803_Cmd{ 0x71, 0x90}
#define LTC6803_CMD_STOWDC_Cell2            LTC6803_Cmd{ 0x72, 0x99}
#define LTC6803_CMD_STOWDC_Cell3            LTC6803_Cmd{ 0x73, 0x9E}
#define LTC6803_CMD_STOWDC_Cell4            LTC6803_Cmd{ 0x74, 0x8B}
#define LTC6803_CMD_STOWDC_Cell5            LTC6803_Cmd{ 0x75, 0x8C}
#define LTC6803_CMD_STOWDC_Cell6            LTC6803_Cmd{ 0x76, 0x85}
#define LTC6803_CMD_STOWDC_Cell7            LTC6803_Cmd{ 0x77, 0x82}
#define LTC6803_CMD_STOWDC_Cell8            LTC6803_Cmd{ 0x78, 0xAF}
#define LTC6803_CMD_STOWDC_Cell9            LTC6803_Cmd{ 0x79, 0xA8}
#define LTC6803_CMD_STOWDC_Cell10           LTC6803_Cmd{ 0x7A, 0xA1}
#define LTC6803_CMD_STOWDC_Cell11           LTC6803_Cmd{ 0x7B, 0xA6}
#define LTC6803_CMD_STOWDC_Cell12           LTC6803_Cmd{ 0x7C, 0xB3}

#endif  // __LTC6803_CMDS_H
