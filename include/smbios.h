#ifndef SMBIOS_H
#define SMBIOS_H

#include "types.h"
#include "../src/drivers/smbios.c"

// Function to print SMBIOS information
void printSMBIOSInfo(unsigned char *mem);

// Function to write SMBIOS information
void writeSMBIOSInfo(unsigned char *mem);

void initsmbiosfind();

#endif
