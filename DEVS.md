# CatK for Developers

Here's how stuff's laid out:
| Directory                                                                    | Purpose |
| ---------- | ---------------------------------------------------------------------------- |
| config               | Linker scripts and kernel                                                                                                                                |
| src/kernel           | The actual kernel, it's headers and libk                                                                                                                 |
| src/lib              | Stuff for userspace development, like libc                                                                                                               |
| src/utils            | Utilities that should exist in the filesystem                                                                                                            |
| skeleton             | The skeleton for the system root directory, for when we move to having an FS                                                                             |