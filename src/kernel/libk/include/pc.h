#pragma once

#include <stdint.h>

uint8_t inportb(uint16_t port);
void outportb(uint16_t port, uint8_t val);