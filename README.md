# CatK https://catk.neocities.org/
a simple Unix-like kernel, written almost entirely in C. Using a cute kitty mascot named CatK!

![Screenshot from 2023-11-11 17-34-55](https://github.com/Rodmatronic/CatK/assets/105672808/3b54d12a-9199-4b8e-9f01-336d34c927aa)
![Screenshot from 2023-11-02 21-39-49](https://github.com/Rodmatronic/CatK/assets/105672808/498231ad-fd9b-4306-80c5-548d0d8023d9)

# To compile from source
Run `make` at the root of the project, where the Makefile is.

# Run the ISO/Binary
Run `qemu-system-x86_64 -cdrom //CURRENT-VERSION.iso` Where the .iso is located. (usually /out on builds.)

Run `qemu-system-x86_64 -kernel //CURRENT-VERSION.bin` Where the .bin is located. (usually not in builds, but from the release downloads)
