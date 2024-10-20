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
    Cmd WRCFG                               { 0x01, 0xC7};
    Cmd RDCFG                               { 0x02, 0xDE};
    Cmd RDCV                                { 0x04, 0xDC};
    Cmd RDCVA                               { 0x06, 0xD2};
    Cmd RDCVB                               { 0x08, 0xF8};
    Cmd RDCVC                               { 0x0A, 0xF6};
    Cmd RDFLG                               { 0x0C, 0xE4};
    Cmd RDTMP                               { 0x0E, 0xEA};
    Cmd STCVAD_ALL                          { 0x10, 0xB0};
    Cmd STCVAD_Cell1                        { 0x11, 0xB7};
    Cmd STCVAD_Cell2                        { 0x12, 0xBE};
    Cmd STCVAD_Cell3                        { 0x13, 0xB9};
    Cmd STCVAD_Cell4                        { 0x14, 0xAC};
    Cmd STCVAD_Cell5                        { 0x15, 0xAB};
    Cmd STCVAD_Cell6                        { 0x16, 0xA2};
    Cmd STCVAD_Cell7                        { 0x17, 0xA5};
    Cmd STCVAD_Cell8                        { 0x18, 0x88};
    Cmd STCVAD_Cell9                        { 0x19, 0x8F};
    Cmd STCVAD_Cell10                       { 0x1A, 0x86};
    Cmd STCVAD_Cell11                       { 0x1B, 0x81};
    Cmd STCVAD_Cell12                       { 0x1C, 0x94};
    Cmd STCVAD_Clear                        { 0x1D, 0x93};
    Cmd STCVAD_SelfTest1                    { 0x1E, 0x9A};
    Cmd STCVAD_SelfTest2                    { 0x1F, 0x9D};
    Cmd STOWAD_ALL                          { 0x20, 0x20};
    Cmd STOWAD_Cell1                        { 0x21, 0x27};
    Cmd STOWAD_Cell2                        { 0x22, 0x2E};
    Cmd STOWAD_Cell3                        { 0x23, 0x29};
    Cmd STOWAD_Cell4                        { 0x24, 0x3C};
    Cmd STOWAD_Cell5                        { 0x25, 0x3B};
    Cmd STOWAD_Cell6                        { 0x26, 0x32};
    Cmd STOWAD_Cell7                        { 0x27, 0x35};
    Cmd STOWAD_Cell8                        { 0x28, 0x18};
    Cmd STOWAD_Cell9                        { 0x29, 0x1F};
    Cmd STOWAD_Cell10                       { 0x2A, 0x16};
    Cmd STOWAD_Cell11                       { 0x2B, 0x11};
    Cmd STOWAD_Cell12                       { 0x2C, 0x04};
    Cmd STTMPAD_ALL                         { 0x30, 0x50};
    Cmd STTMPAD_External1                   { 0x31, 0x57};
    Cmd STTMPAD_External2                   { 0x32, 0x5E};
    Cmd STTMPAD_Internal                    { 0x33, 0x59};
    Cmd STTMPAD_SelfTest1                   { 0x3E, 0x7A};
    Cmd STTMPAD_SelfTest2                   { 0x3F, 0x7D};
    Cmd PLADC                               { 0x40, 0x07};
    Cmd PLINT                               { 0x50, 0x77};
    Cmd DAGN                                { 0x52, 0x79};
    Cmd RDDGNR                              { 0x54, 0x6B};
    Cmd STCVDC_ALL                          { 0x60, 0xE7};
    Cmd STCVDC_Cell1                        { 0x61, 0xE0};
    Cmd STCVDC_Cell2                        { 0x62, 0xE9};
    Cmd STCVDC_Cell3                        { 0x63, 0xEE};
    Cmd STCVDC_Cell4                        { 0x64, 0xFB};
    Cmd STCVDC_Cell5                        { 0x65, 0xFC};
    Cmd STCVDC_Cell6                        { 0x66, 0xF5};
    Cmd STCVDC_Cell7                        { 0x67, 0xF2};
    Cmd STCVDC_Cell8                        { 0x68, 0xDF};
    Cmd STCVDC_Cell9                        { 0x69, 0xD8};
    Cmd STCVDC_Cell10                       { 0x6A, 0xD1};
    Cmd STCVDC_Cell11                       { 0x6B, 0xD6};
    Cmd STCVDC_Cell12                       { 0x6C, 0xC3};
    Cmd STOWDC_ALL                          { 0x70, 0x97};
    Cmd STOWDC_Cell1                        { 0x71, 0x90};
    Cmd STOWDC_Cell2                        { 0x72, 0x99};
    Cmd STOWDC_Cell3                        { 0x73, 0x9E};
    Cmd STOWDC_Cell4                        { 0x74, 0x8B};
    Cmd STOWDC_Cell5                        { 0x75, 0x8C};
    Cmd STOWDC_Cell6                        { 0x76, 0x85};
    Cmd STOWDC_Cell7                        { 0x77, 0x82};
    Cmd STOWDC_Cell8                        { 0x78, 0xAF};
    Cmd STOWDC_Cell9                        { 0x79, 0xA8};
    Cmd STOWDC_Cell10                       { 0x7A, 0xA1};
    Cmd STOWDC_Cell11                       { 0x7B, 0xA6};
    Cmd STOWDC_Cell12                       { 0x7C, 0xB3};
}
