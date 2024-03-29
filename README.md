<h1 align=center><a href="https://catk.neocities.org">CatK</a></h1>

<p align=center>a simple Unix-like kernel made in C, with a cute kitty (CatK) to go with it</p>

---

![image](https://github.com/Rodmatronic/CatK/assets/105672808/2ca8018b-3506-49c7-a515-2d455a4a9806)
![image](https://github.com/Rodmatronic/CatK/assets/105672808/3638a369-b576-4271-8f9c-a87ac844a8c1)

**Table of Contents**
- [What is CatK?](#what-is-catk)
- [Compiling it](#compiling-it)
- [Running it](#running-it)
- [Debugging it](#debugging-it)
- [Contributing](#contributing)
- [Support/Issues](#supportissues)
- [Get in touch](#get-in-touch)
- [Licensing](#licensing)

## What is CatK?
CatK is a kernel taking inspiration from FreeBSD and similar Unix-like systems written almost completely in C.\
CatK is also the cat that represents this project.

We love CatK.

## Compiling it
You'll need:
- a unix-like system/environment (preferably Linux)
  - compiling on Windows is possible provided a unix-style environment (uutils, mingw, etc). you cannot build any ISOs however (credit [@Ruthenic](https://github.com/Ruthenic))
- clang 14+ with support for i686-pc-none-elf (so, most installs)
- LLVM lld (or GNU ld, if you set it manually)
- grub-mkrescue (This usually comes with GRUB) and xorriso
- the NASM assembler
- GNU make

Afterwards, it's very simple to compile. All you need to do is run `make`, and your ISO will be automagically prepared in the out/ folder.

You can skip ISO building and multiboot signature checking by using `SKIP_ISO=1 make` and `SKIP_MB_CHECK=1 make` respectively.

These are implied and forced on Windows, as they cannot be performed.

If you prefer, you can compile with multiple threads with `make -j$(nproc)`. This will use as many threads as your CPU has available.

## Running it
You can use essentially any virtualizer you have. We prefer QEMU or VirtualBox.

If you wanna use QEMU, you can start the ISO up with `qemu-system-i386 -cdrom /path/to/catkernel.iso` (hint: it's usually in out/)\
Also, if you're using QEMU, you can directly start the kernel with `qemu-system-i386 -kernel /path/to/catkernel.bin` (hint: it's usually in out/isodir/boot, or just out/ if you are not building the ISO)

See "Debugging it" for compiling/running with a debugger (GDB/LLDB).

## Debugging it
This guide assumes you already have a familiarity with GDB/LLDB (we can't assist with using your debugger of choice.)

First, build CatK with debug symbols enabled (`make clean && DEBUG=1 make`).

Next, run QEMU like this: `qemu-system-i386 -s -S -kernel /path/to/catkernel.bin`\
QEMU will wait for you to attach a GDB-compatible debugger and to `continue`. Let's do that.

Run `gdb /path/to/catkernel.bin` or `lldb /path/to/catkernel.bin`. You will be dropped to your debugger's shell.

In that shell, run `target remote localhost:1234` for GDB, and `gdb-remote localhost:1234` for LLDB.

Run `c` to tell the VM to start running.

As a quick crash course, `CTRL + C` pauses execution while continuing, `b function_name` will let you break when the specified function is hit. `s` will let you step by instructions. `c` will continue as normal until the next breakpoint.

Happy travels.

## Contributing
Contributions are accepted and encouraged!

Please try to follow the coding style (we have so much cleaning up to do...) that the rest of the code follows.

Please discuss changes you intend on making if they are large in the dev section of our Discord (see "Get in touch").

## Support/Issues
If you have a question or a small issue/something you don't quite understand, it's best to just join our Discord. See "Get in touch" for that.
Otherwise, if it's a large bug that you think you can provide good info on, it's best to create a GitHub issue as that helps us track the problem better.

## Get in touch
Currently, the best way to get in touch with the developers, to discuss contributions, and generally be a part of the CatK community is to join our [Discord](https://discord.gg/6U3pQEkTT7).

It's quite nice there.

## Licensing
CatK is licensed under the GNU General Public License version 3. See LICENSE.
