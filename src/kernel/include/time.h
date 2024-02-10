#pragma once

#define PIT_CHANNEL_0 0x40
#define PIT_COMMAND   0x43
#define PIT_FREQUENCY 1193182
#define DESIRED_ACCURACY 0.0001 // 0.0001 seconds

void time_init();
void current_date();
void current_time();
void sleep(unsigned int seconds);
void cal();
void current_full_date();
void pit_interrupt_handler();
void init_pit();