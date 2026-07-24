# 05 — ioctl and the misc Device (stub)

> 🔜 This module gets fully built out when you finish 04. Tell Claude Code
> "I finished 04" and it will be generated against what you learned.

What it will cover:

- Why read/write isn't enough: out-of-band commands (think: "set baud rate",
  "reset FIFO") and why `ioctl` is the kernel's escape hatch
- Designing an ioctl interface: `_IO`, `_IOR`, `_IOW`, `_IOWR` macros, magic
  numbers, and a shared header between kernel and userspace
- A small userspace C program that opens `/dev/...` and drives the ioctls —
  your first two-sided (kernel + user) project
- `struct miscdevice`: the shortcut that replaces half of module 04's init
  boilerplate with one `misc_register()` call

Planned lab: extend `kbuf` with ioctls to CLEAR the buffer, query its size, and
toggle a read-only mode — plus a userspace tool to invoke them.
