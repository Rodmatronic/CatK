#include <catk/core.h>
#include <catk/printk.h>
#include <lib/common.h>
#include <stdint.h>

inline void critical_enter(void)
{
  asm volatile("cli");
}

inline void critical_exit(void)
{
  asm volatile("sti");
}

static void cpuid(uint32_t code, uint32_t * a, uint32_t * b, uint32_t * c, uint32_t * d) 
{
  asm volatile("cpuid"
                     : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                     : "a"(code));
}

static int cpugetbrand(void);
static void cpugetvendor(void);

// This will display a very BSD-like CPU dump 'n stuff.
void cpu_dump_all_info(void)
{
  cpugetbrand();
  cpugetvendor();
}

static int cpugetbrand(void)
{
  uint32_t reg_values[12];
  char brand_string[49];
  uint32_t eax, ebx, ecx, edx;
  cpuid(0x80000000, &reg_values[0], &reg_values[1], &reg_values[2], &reg_values[3]);
  if (eax < 0x80000004)
    return 0;
  cpuid(0x80000002, &reg_values[0], &reg_values[1], &reg_values[2], &reg_values[3]);
  cpuid(0x80000003, &reg_values[4], &reg_values[5], &reg_values[6], &reg_values[7]);
  cpuid(0x80000004, &reg_values[8], &reg_values[9], &reg_values[10], &reg_values[11]);
  // im guessing that these register values hold ascii character codes to form the string
  memcpy(brand_string, reg_values, sizeof(reg_values));
  brand_string[48] = '\0';
  printk("CPU: %s\n", brand_string);
  return 1;
}

static void cpugetvendor(void)
{
  uint32_t eax, ebx, ecx, edx;
  
  // Call cpuid with code 0 to get the vendor ID string
  cpuid(0, &eax, &ebx, &ecx, &edx);

  char vendor[13];
  *((uint32_t *)vendor) = ebx;
  *((uint32_t *)(vendor + 4)) = edx;
  *((uint32_t *)(vendor + 8)) = ecx;
  vendor[12] = '\0';

  printk("CPU Origin: %s\n", vendor);
}