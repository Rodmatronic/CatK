# CatK
a simple Unix-like kernel, written almost entirely in C.

![PreBoot](https://github.com/Rodmatronic/CatK/assets/105672808/cfc89e6a-e1e2-4ca9-b03f-4bbd60b3fef3)
![sh](https://github.com/Rodmatronic/CatK/assets/105672808/7992424c-799c-4d82-8fed-60ab8b282ad5)

# To compile from source
Run `make` at the root of the project, where the Makefile is.

# Run the ISO/Binary
Run `qemu-system-x86_64 -cdrom //CURRENT-VERSION.iso` Where the .iso is located. (usually /out on builds.)

Run `qemu-system-x86_64 -kernel //CURRENT-VERSION.BIN` Where the .bin is located. (usually not in builds, but from the release downloads)
