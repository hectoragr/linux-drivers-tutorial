# 00 — Setup & Orientation

Goal: verify your toolchain, understand what "building a kernel module" actually
means, and learn the handful of commands you'll use in every single module.

## 0.1 What you're building against

Userspace programs link against libc and call the kernel through syscalls. A kernel
module is different: it's a `.ko` (kernel object) file that gets **linked into the
running kernel** at load time. That has three big consequences you should internalize
now, because they explain almost every "weird" thing about kernel code:

1. **No libc.** No `printf`, no `malloc`, no `pthread`. The kernel has its own
   equivalents (`printk`, `kmalloc`, kthreads) with different semantics.
2. **No memory protection for you.** A wild pointer in userspace segfaults your
   process. In the kernel it corrupts *the kernel* — best case an "oops", worst
   case silent data corruption or a frozen machine.
3. **ABI is unstable by design.** A module built for kernel 6.8.0-136 will refuse
   to load on 6.8.0-134. That's why you need the *headers for your exact running
   kernel*, and why out-of-tree drivers (like your `snd-hda-codec-cs8409`) use DKMS
   to rebuild themselves on every kernel update.

Coming from microcontrollers: think of the kernel as a giant, preemptive,
multi-core RTOS where your "task" shares one address space with everything else,
and the scheduler, ISRs, and other cores are all running *your* code concurrently.
That mental model will serve you well in module 07.

## 0.2 Verify your environment

Run the check script:

```bash
./check-setup.sh
```

It verifies: gcc, make, kernel headers matching `uname -r`, and the tools used
throughout the tutorial. If anything is missing on Mint/Ubuntu:

```bash
sudo apt install build-essential linux-headers-$(uname -r) kmod
```

## 0.3 Commands you'll use constantly

| Command | What it does |
|---------|--------------|
| `uname -r` | Running kernel version — headers must match this exactly |
| `lsmod` | List loaded modules (reads `/proc/modules`) |
| `sudo insmod foo.ko` | Load a module file (no dependency resolution) |
| `sudo rmmod foo` | Unload a module |
| `sudo modprobe foo` | Load an *installed* module + its dependencies |
| `modinfo foo.ko` | Show a module's metadata (license, params, vermagic) |
| `sudo dmesg -w` | Follow the kernel log live — keep this open in a second terminal, always |
| `journalctl -k -f` | Same thing via systemd, survives reboots |

Try a few right now on a real driver you already run:

```bash
lsmod | head
modinfo snd_hda_codec_cs8409   # your MacBook audio driver!
sudo dmesg | tail -20
```

## 0.4 Where things live

- `/lib/modules/$(uname -r)/` — installed modules for the running kernel
- `/lib/modules/$(uname -r)/build` — symlink to the matching headers; **this is
  what your Makefiles point at**
- `/usr/src/` — the actual header trees
- `/proc/` and `/sys/` — the kernel talking back to you; you'll *write* entries
  here in module 06

## Checkpoint

Before moving on you should be able to answer:

1. Why must module headers match the running kernel exactly, when a userspace
   binary from 2010 still runs today?
2. What's the difference between `insmod` and `modprobe`?
3. `dmesg` shows messages from where, physically? (Hint: it's a ring buffer in
   kernel memory — what happens when it fills up?)

Next: [01-c-for-kernel](../01-c-for-kernel/) — a C refresher, but through the
kernel's idioms rather than a generic recap.
