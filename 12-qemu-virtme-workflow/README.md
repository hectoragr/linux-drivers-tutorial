# 12 — Crash-Safe Development: QEMU + virtme-ng (stub)

> 🔜 Built out when you get here — tell Claude Code.

From here on, mistakes get expensive on real hardware. This module sets up the
workflow professional kernel developers use: build a kernel, boot it in a VM
that shares your filesystem, crash it freely, iterate in seconds.

What it will cover:

- `virtme-ng`: boot *your current kernel* (or a custom build) in QEMU with your
  home directory mounted — `vng --run`, load a module, panic, be back in 5 s
- Building a kernel from source: `menuconfig` minimal configs, `make -j`,
  what all those CONFIG_ options mean for driver writers
- A deliberate `BUG()` module — enjoy your first guilt-free kernel panic
- gdb against a running kernel (`qemu -s` + `vmlinux`) — real source-level
  kernel debugging, like the JTAG debugging you know from microcontrollers

This module is also the prerequisite for 14 (Rust), since Rust-for-Linux needs
a custom-configured kernel.
