/**
 * commands for the ltc6903
 * 
 * - pg21
 * - oddly enough this dosent already exist in the libs, they hardcode the values
 */

#include <stdint.h>
//#include <type_traits>  // for std::is_standard_layout

namespace LTC6803_Cmd {
    union Cmd {
        struct Cmd_s{
            uint8_t code;       // command code
            uint8_t pec;        // packet error code
        } cmd;
       // static_assert(std::is_standard_layout<Cmd_s>::value);

        uint8_t arr[2];
    };

    //          command                       code  pec
    const Cmd WRCFG                         { 0x01, 0xC7};
    const Cmd RDCFG                         { 0x02, 0xDE};
    const Cmd RDCV                          { 0x04, 0xDC};
    const Cmd RDCVA                         { 0x06, 0xD2};
    const Cmd RDCVB                         { 0x08, 0xF8};
    const Cmd RDCVC                         { 0x0A, 0xF6};
    const Cmd RDFLG                         { 0x0C, 0xE4};
    const Cmd RDTMP                         { 0x0E, 0xEA};
    const Cmd STCVAD_ALL                    { 0x10, 0xB0};
    const Cmd STCVAD_Cell1                  { 0x11, 0xB7};
    const Cmd STCVAD_Cell2                  { 0x12, 0xBE};
    const Cmd STCVAD_Cell3                  { 0x13, 0xB9};
    const Cmd STCVAD_Cell4                  { 0x14, 0xAC};
    const Cmd STCVAD_Cell5                  { 0x15, 0xAB};
    const Cmd STCVAD_Cell6                  { 0x16, 0xA2};
    const Cmd STCVAD_Cell7                  { 0x17, 0xA5};
    const Cmd STCVAD_Cell8                  { 0x18, 0x88};
    const Cmd STCVAD_Cell9                  { 0x19, 0x8F};
    const Cmd STCVAD_Cell10                 { 0x1A, 0x86};
    const Cmd STCVAD_Cell11                 { 0x1B, 0x81};
    const Cmd STCVAD_Cell12                 { 0x1C, 0x94};
    const Cmd STCVAD_Clear                  { 0x1D, 0x93};
    const Cmd STCVAD_SelfTest1              { 0x1E, 0x9A};
    const Cmd STCVAD_SelfTest2              { 0x1F, 0x9D};
    const Cmd STOWAD_ALL                    { 0x20, 0x20};
    const Cmd STOWAD_Cell1                  { 0x21, 0x27};
    const Cmd STOWAD_Cell2                  { 0x22, 0x2E};
    const Cmd STOWAD_Cell3                  { 0x23, 0x29};
    const Cmd STOWAD_Cell4                  { 0x24, 0x3C};
    const Cmd STOWAD_Cell5                  { 0x25, 0x3B};
    const Cmd STOWAD_Cell6                  { 0x26, 0x32};
    const Cmd STOWAD_Cell7                  { 0x27, 0x35};
    const Cmd STOWAD_Cell8                  { 0x28, 0x18};
    const Cmd STOWAD_Cell9                  { 0x29, 0x1F};
    const Cmd STOWAD_Cell10                 { 0x2A, 0x16};
    const Cmd STOWAD_Cell11                 { 0x2B, 0x11};
    const Cmd STOWAD_Cell12                 { 0x2C, 0x04};
    const Cmd STTMPAD_ALL                   { 0x30, 0x50};
    const Cmd STTMPAD_External1             { 0x31, 0x57};
    const Cmd STTMPAD_External2             { 0x32, 0x5E};
    const Cmd STTMPAD_Internal              { 0x33, 0x59};
    const Cmd STTMPAD_SelfTest1             { 0x3E, 0x7A};
    const Cmd STTMPAD_SelfTest2             { 0x3F, 0x7D};
    const Cmd PLADC                         { 0x40, 0x07};
    const Cmd PLINT                         { 0x50, 0x77};
    const Cmd DAGN                          { 0x52, 0x79};
    const Cmd RDDGNR                        { 0x54, 0x6B};
    const Cmd STCVDC_ALL                    { 0x60, 0xE7};
    const Cmd STCVDC_Cell1                  { 0x61, 0xE0};
    const Cmd STCVDC_Cell2                  { 0x62, 0xE9};
    const Cmd STCVDC_Cell3                  { 0x63, 0xEE};
    const Cmd STCVDC_Cell4                  { 0x64, 0xFB};
    const Cmd STCVDC_Cell5                  { 0x65, 0xFC};
    const Cmd STCVDC_Cell6                  { 0x66, 0xF5};
    const Cmd STCVDC_Cell7                  { 0x67, 0xF2};
    const Cmd STCVDC_Cell8                  { 0x68, 0xDF};
    const Cmd STCVDC_Cell9                  { 0x69, 0xD8};
    const Cmd STCVDC_Cell10                 { 0x6A, 0xD1};
    const Cmd STCVDC_Cell11                 { 0x6B, 0xD6};
    const Cmd STCVDC_Cell12                 { 0x6C, 0xC3};
    const Cmd STOWDC_ALL                    { 0x70, 0x97};
    const Cmd STOWDC_Cell1                  { 0x71, 0x90};
    const Cmd STOWDC_Cell2                  { 0x72, 0x99};
    const Cmd STOWDC_Cell3                  { 0x73, 0x9E};
    const Cmd STOWDC_Cell4                  { 0x74, 0x8B};
    const Cmd STOWDC_Cell5                  { 0x75, 0x8C};
    const Cmd STOWDC_Cell6                  { 0x76, 0x85};
    const Cmd STOWDC_Cell7                  { 0x77, 0x82};
    const Cmd STOWDC_Cell8                  { 0x78, 0xAF};
    const Cmd STOWDC_Cell9                  { 0x79, 0xA8};
    const Cmd STOWDC_Cell10                 { 0x7A, 0xA1};
    const Cmd STOWDC_Cell11                 { 0x7B, 0xA6};
    const Cmd STOWDC_Cell12                 { 0x7C, 0xB3};
}
