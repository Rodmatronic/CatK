#include "types.h"
#include "io_ports.h"
#define ACPI_RESET 0x06

void reboot() {
    // This inline assembly code sends the ACPI shutdown command.
__asm__ __volatile__("int $0x19");
}
