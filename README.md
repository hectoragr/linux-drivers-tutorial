# Linux Drivers Tutorial

A hands-on, progressive tutorial for (re)learning Linux kernel driver development —
written for an Electronics Engineer with a microcontroller/C background who wants to
get back into low-level engineering.

Every module is a directory with a `README.md` (theory + walkthrough), source code
that **compiles on this machine** (kernel 6.8, gcc 11), and exercises. Work through
them in order; each one builds on the previous.

## How to use this repo

1. `cd` into a module directory and read its `README.md`.
2. Build (`make`), load, poke, and unload the code as the README describes.
3. Do the exercises at the end — they're where the learning actually happens.
4. When you finish a module (or get stuck), come back to Claude Code and say so —
   the next modules get fleshed out as you advance, tailored to what you found
   easy or hard.

> ⚠️ **Safety note:** Parts 0–1 are safe to run on your host machine — the modules
> are tiny and self-contained. From Part 2 onward (interrupts, memory games,
> platform devices) a bug can freeze or panic your machine. Module 12 sets up a
> QEMU-based workflow precisely so you can crash kernels without consequences.
> Until then: save your work before `insmod`. 🙂

## Curriculum

### Part 0 — Foundations
| Module | Topic | Status |
|--------|-------|--------|
| [00-setup](00-setup/) | Toolchain, kernel headers, essential commands | ✅ ready |
| [01-c-for-kernel](01-c-for-kernel/) | C recap through a kernel lens: function-pointer ops tables, `container_of`, intrusive lists, bit twiddling | ✅ ready |

### Part 1 — Classic kernel module fundamentals
| Module | Topic | Status |
|--------|-------|--------|
| [02-hello-module](02-hello-module/) | Your first module: `module_init/exit`, `printk`, kbuild, insmod/rmmod | ✅ ready |
| [03-module-parameters](03-module-parameters/) | `module_param`, sysfs exposure, licensing & metadata | ✅ ready |
| [04-character-device](04-character-device/) | Char devices: major/minor, `file_operations`, `cdev`, `copy_to/from_user`, udev | ✅ ready |
| [05-ioctl-and-misc](05-ioctl-and-misc/) | `ioctl` command interface, the `misc` device shortcut | 🔜 stub |
| [06-procfs-sysfs](06-procfs-sysfs/) | Exposing driver state via `/proc` and `/sys` | 🔜 stub |
| [07-concurrency](07-concurrency/) | Spinlocks, mutexes, atomics, wait queues, blocking I/O | 🔜 stub |
| [08-interrupts-deferred-work](08-interrupts-deferred-work/) | IRQ handlers, threaded IRQs, timers, workqueues | 🔜 stub |
| [09-memory-and-mmap](09-memory-and-mmap/) | `kmalloc`/`vmalloc`, DMA basics, implementing `mmap` | 🔜 stub |

### Part 2 — Real driver models
| Module | Topic | Status |
|--------|-------|--------|
| [10-platform-drivers-devicetree](10-platform-drivers-devicetree/) | The driver model: buses, `platform_driver`, probe/remove, Device Tree | 🔜 stub |
| [11-debugging-and-tooling](11-debugging-and-tooling/) | `checkpatch`, sparse, dynamic debug, ftrace, `/proc/kallsyms`, oops decoding | 🔜 stub |

### Part 3 — Modern kernel development
| Module | Topic | Status |
|--------|-------|--------|
| [12-qemu-virtme-workflow](12-qemu-virtme-workflow/) | Crash-safe development: QEMU + virtme-ng, custom kernels | 🔜 stub |
| [13-ebpf](13-ebpf/) | eBPF: tracing and observability without writing modules | 🔜 stub |
| [14-rust-for-linux](14-rust-for-linux/) | Rust for Linux: building a Rust kernel module | 🔜 stub |

## This machine

Everything is written against what's actually installed here:

- Linux Mint, kernel `6.8.0-136-generic` (HWE), headers installed
- gcc 11.4, GNU Make 4.3
- Rust toolchain: not yet installed — module 14 covers installing it via rustup

## References worth keeping open

- [Linux Device Drivers, 3rd ed. (LDD3)](https://lwn.net/Kernel/LDD3/) — free, dated (2005) but the pedagogy is unmatched; this tutorial modernizes its examples
- [The Linux Kernel documentation](https://docs.kernel.org/) — especially *Driver implementer's API guide*
- [Kernel source for your running kernel](file:///usr/src/linux-hwe-6.8-headers-6.8.0-136/) — grep it constantly; the source is the real documentation
- [Rust for Linux](https://rust-for-linux.com/)
