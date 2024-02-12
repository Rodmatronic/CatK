#pragma once
#include "stdint.h"

/*
 *                          The Catkernel configuration file                                
 *
 *        These are the configuration options that should be baked into the kernel.
 *    This file contains static values (arch, ver, term) as well as universal variables,
 *               such as memory, the sytem counter, and the randomness seed.
 *   Make sure you know what you are editing! These values are heavily used by the kernel
 */ 

#define	KERNEL_BOOT_TAG	"---<<CATKERNEL BOOT>>---"   // FreeBSD style dmesg message.

//------------------------------------------------

static char * sys_arch = "i386-x86";                 // The arch that CatK should be targeting
static char * sys_term = "80x25";                    // The VGA terminal resolution
static char * sys_sesh = "/bin/k_sh";                // The default shell session
static char * sys_ver = "0.074";                     // The system version
static char * sys_home = "/home/root";               // The home directory
static char * sys_name = "Catkernel";                // The system's name
static char * sys_codename = "IT'S ALIVE!!";         // CatK's codename
static char * sys_user = "root";                     // The system's default user (should always be root!)
static char * sys_mountpoint = "/";                  // The system's mountpoint (should always be "/")

//------------------------------------------------   

extern char cmdline[256];                            // The CMDLINE given by Multiboot
extern char loader[256];                             // The Bootloader name
extern char cpubrand[256];                           // The CPU Brand. (for example, GenuineIntel)
extern char cpumodel[256];                           // The CPU Model. (for example, Intel Xeon E3-12xx v2 (Ivy Bridge))
extern char pwd[1024];                               // The system's universal PWD (single user mode)

//------------------------------------------------

extern uint32_t counter;                             // This is the counter that should be started when the kernel starts
extern uint32_t cpulogicores;                        // The amount of logical cores reported by cpuid_info()
extern uint64_t seed;                                // The random seed that should be set by entropy
extern uint64_t mem;                                 // The amount of memory avalible to the system in MB
extern uint64_t memreal;                             // The amount of memory avalible to the system in bytes
extern uint64_t bmem;                                // The BIOS memory reported in KB