#ifndef TIME_H
#define TIME_H

// I/O Ports for RTC
#define RTC_ADDRESS_PORT 0x70
#define RTC_DATA_PORT 0x71

// RTC Register Addresses
#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS 0x04
#define RTC_DAY_OF_MONTH 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09

#include "types.h"

extern uint8 seconds, minutes, hours, day, month, year;

extern uint8 seed;

void GetCurrentTime(); 
uint8 read_RTC_register(uint8 reg);
void write_RTC_register(uint8 reg, uint8 value);
#endif

