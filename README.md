# CatK
a simple Unix-like kernel, written almost entirely in C.

# To compile from source
Run `make` at the root of the project, where the Makefile is.

# Run the ISO/Binary
Run `qemu-system-x86_64 -cdrom //CURRENT-VERSION.iso` Where the .iso is located. (usually /out on builds.)

Run `qemu-system-x86_64 -kernel //CURRENT-VERSION.BIN` Where the .bin is located. (usually not in builds, but from the release downloads)
