/**
 * supportive functions for LTC6803
 */

#ifndef __LTC6803_SUPPORT_H__
#define __LTC6803_SUPPORT_H__

#include "Environment.h"

#include <stdint.h>


uint16_t ADCRangeTomV(uint16_t adc_output);

uint8_t pec8calc(uint8_t length, uint8_t* data);

#endif // __LTC6803_SUPPORT_H__