# CatK https://catk.neocities.org/
a simple Unix-like kernel, written almost entirely in C. Using a cute kitty mascot named CatK!

![Screenshot from 2023-11-19 21-36-50](https://github.com/Rodmatronic/CatK/assets/105672808/a9d59246-a663-4215-bf24-0b12030b0529)
![Screenshot from 2023-11-19 21-38-11](https://github.com/Rodmatronic/CatK/assets/105672808/3931c30a-88f2-4ffd-bb77-c921b78ad141)

# To compile from source
Run `make` at the root of the project, where the Makefile is.

# Run the ISO/Binary
Run `qemu-system-x86_64 -cdrom //CURRENT-VERSION.iso` Where the .iso is located. (usually /out on builds.)

Run `qemu-system-x86_64 -kernel //CURRENT-VERSION.bin` Where the .bin is located. (usually not in builds, but from the release downloads)
