#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <term.h>
#include "term.h"
#include "printk.h"
#include "panic.h"
#include "kernel.h"
#include "time.h"
#include "pc.h"
#include "config.h"
#include "isr.h"

int century_register = 0x00;                                // Set by ACPI table parsing code if possible
 
unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;
unsigned char month;
unsigned int year;

const char *months[] = {
    "Jan", "Feb", "Mar", "Apr",
    "May", "Jun", "Jul", "Aug",
    "Sep", "Oct", "Nov", "Dec"
};

const char *daysOfWeek[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

int dayOfWeek() {
    // Determine the day of the week for the current date
    return (day + 2 * (13 * (month + 1) / 5) + (year % 100) + ((year % 100) / 4) + ((year / 100) / 4) - 2 * (year / 100) + 700) % 7;
}

enum {
      cmos_address = 0x70,
      cmos_data    = 0x71
};
 
int get_update_in_progress_flag() {
      outportb(cmos_address, 0x0A);
      return (inportb(cmos_data) & 0x80);
}
 
unsigned char get_RTC_register(int reg) {
      outportb(cmos_address, reg);
      return inportb(cmos_data);
}
 
void read_rtc() {
      unsigned char century = '\0';
      unsigned char last_second;
      unsigned char last_minute;
      unsigned char last_hour;
      unsigned char last_day;
      unsigned char last_month;
      unsigned char last_year;
      unsigned char last_century;
      unsigned char registerB;
 
      // Note: This uses the "read registers until you get the same values twice in a row" technique
      //       to avoid getting dodgy/inconsistent values due to RTC updates
 
      while (get_update_in_progress_flag());                // Make sure an update isn't in progress
      second = get_RTC_register(0x00);
      minute = get_RTC_register(0x02);
      hour = get_RTC_register(0x04);
      day = get_RTC_register(0x07);
      month = get_RTC_register(0x08);
      year = get_RTC_register(0x09);
      if(century_register != 0) {
            century = get_RTC_register(century_register);
      }
 
      do {
            last_second = second;
            last_minute = minute;
            last_hour = hour;
            last_day = day;
            last_month = month;
            last_year = year;
            last_century = century;
 
            while (get_update_in_progress_flag());           // Make sure an update isn't in progress
            second = get_RTC_register(0x00);
            minute = get_RTC_register(0x02);
            hour = get_RTC_register(0x04);
            day = get_RTC_register(0x07);
            month = get_RTC_register(0x08);
            year = get_RTC_register(0x09);
            if(century_register != 0) {
                  century = get_RTC_register(century_register);
            }
      } while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
               (last_day != day) || (last_month != month) || (last_year != year) ||
               (last_century != century) );
 
      registerB = get_RTC_register(0x0B);
 
      // Convert BCD to binary values if necessary
 
      if (!(registerB & 0x04)) {
            second = (second & 0x0F) + ((second / 16) * 10);
            minute = (minute & 0x0F) + ((minute / 16) * 10);
            hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
            day = (day & 0x0F) + ((day / 16) * 10);
            month = (month & 0x0F) + ((month / 16) * 10);
            year = (year & 0x0F) + ((year / 16) * 10);
            if(century_register != 0) {
                  century = (century & 0x0F) + ((century / 16) * 10);
            }
      }
 
      // Convert 12 hour clock to 24 hour clock if necessary
 
      if (!(registerB & 0x02) && (hour & 0x80)) {
            hour = ((hour & 0x7F) + 12) % 24;
      }
 
      // Calculate the full (4-digit) year
 
      if(century_register != 0) {
            year += century * 100;
      } else {
            year += (CURRENT_YEAR / 100) * 100;
            if(year < CURRENT_YEAR) year += 100;
      }
}

void time_init()
{
    printk("time_init: init\n");
    read_rtc();
    printk("%d/%d/%d %d:%d:%d\n", day, month, year, hour, minute, second);
}

int current_time()
{
    read_rtc();
    printk("%d:%d:%d", hour, minute, second);
    return second;
}

void current_date()
{
    read_rtc();
    printk("%d/%d/%d", day, month, year);
}

void current_full_date()
{
    read_rtc();
    printk("%s %s %d %d:%d:%d %d\n", daysOfWeek[dayOfWeek()], months[month - 1], day, hour, minute, second, CURRENT_YEAR);
}

void cal() {
    // Determine the day of the week for the first day of the month
    int dayOfWeek = (day + 2 * (13 * (month + 1) / 5) + (year % 100) + ((year % 100) / 4) + ((year / 100) / 4) - 2 * (year / 100) + 700) % 7;

    printk("      %s %d\n", months[month - 1], CURRENT_YEAR);
    printk("Su Mo Tu We Th Fr Sa\n");
    read_rtc();

    // Print leading spaces for the first week
    for (int i = 0; i < dayOfWeek; i++) {
        printk("   ");
    }

    // Print the days of the month
    for (int i = 1; i <= 31; i++) {
        if (i == day) {
            // Print the current day with a lighter color
            printk("#%d# ", i);
        } else {
            if (i > 9) {
                printk("%d ", i);
            } else {
                printk("%d  ", i);
            }
        }

        // Move to the next line for the next week
        if ((dayOfWeek + i) % 7 == 0) {
            printk("\n");
        }
    }

    printk("\n");
}


void sleep(unsigned int seconds) {
    unsigned int start_seconds, elapsed_seconds;

    read_rtc();
    start_seconds = second + (minute * 60) + (hour * 3600);

    do {
        read_rtc();
        elapsed_seconds = second + (minute * 60) + (hour * 3600) - start_seconds;
    } while (elapsed_seconds < seconds);
}

void pit_interrupt_handler() {
    counter++;
}

void init_pit() {
    printk("init_pit: init\n");
    // Calculate the divisor for the desired accuracy
    uint16_t divisor = (uint16_t)(PIT_FREQUENCY * DESIRED_ACCURACY);

    // Send the command byte to initialize the PIT
    outportb(PIT_COMMAND, 0x34); // Channel 0, lobyte/hibyte access, mode 2 (rate generator), binary mode

    // Send the divisor (low byte, then high byte)
    outportb(PIT_CHANNEL_0, (uint8_t)(divisor & 0xFF)); // Send low byte
    outportb(PIT_CHANNEL_0, (uint8_t)((divisor >> 8) & 0xFF)); // Send high byte

    printk("init_pit: (added to IDT) ");
    isr_register_interrupt_handler(0x20, pit_interrupt_handler);
}