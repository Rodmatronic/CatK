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

inline void halt(void)
{
  asm volatile("hlt");
}

static void cpuid(uint32_t code, uint32_t * a, uint32_t * b, uint32_t * c, uint32_t * d) 
{
  asm volatile("cpuid"
                     : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                     : "a"(code));
}

static uint32_t xorshift32(uint32_t state) {
    // Simple xorshift PRNG
    state ^= (state << 13);
    state ^= (state >> 17);
    state ^= (state << 5);
    return state;
}

int gen_random(void) {
  uint32_t lo1, hi1, lo2, hi2;
  uint32_t entropy1, entropy2;

  // Read time-stamp counter once
  asm volatile ("rdtsc" : "=a"(lo1), "=d"(hi1));

  // Looks dumb, but read time-stamp counter again with a slight delay
  asm volatile ("rdtsc" : "=a"(lo2), "=d"(hi2));
  uint64_t ts_combined = ((uint64_t)hi1 << 32) | lo1;
  uint64_t ts_combined2 = ((uint64_t)hi2 << 32) | lo2;
  uint64_t combined_result = ts_combined ^ ts_combined2;

  // Extract a 32-bit value
  entropy1 = (uint32_t)(combined_result & 0xFFFFFFFF);
  asm volatile ("rdtsc" : "=a"(lo1), "=d"(hi1));
  entropy2 = (uint32_t)(hi1 ^ lo1);

  // Combine the two sources of entropy
  uint32_t mixed_entropy = xorshift32(entropy1 ^ entropy2);

  return (int)(mixed_entropy);

}

static int cpugetbrand(void)
{
  uint32_t reg_values[12];
  char brand_string[49];
  cpuid(0x80000000, &reg_values[0], &reg_values[1], &reg_values[2], &reg_values[3]);
  if (reg_values[0] < 0x80000004)
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

static void cpugetcores(void) {
  uint32_t eax, ebx, ecx, edx;
  cpuid(1, &eax, &ebx, &ecx, &edx);
  int cores = (ebx >> 16) & 0xff;
  if (cores == 0) {
    // If the number of cores is reported as 0, we need to check if hyper-threading is enabled
    cores = (ebx >> 28) & 0x0f;
    if (cores > 0) {
      // Hyper-threading is enabled, so the actual number of cores is half the number of logical processors
      cores *= 2;
    } else {
      // Hyper-threading is not enabled, so the number of cores remains 1
      cores = 1;
    }
  }
  printk("CPU Cores: %d\n", cores);
}

// This will display a very BSD-like CPU dump 'n stuff.
void cpu_dump_all_info(void)
{
  cpugetbrand();
  cpugetvendor();
  cpugetcores();
}