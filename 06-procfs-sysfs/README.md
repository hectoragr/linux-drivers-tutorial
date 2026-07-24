# 06 — procfs and sysfs (stub)

> 🔜 Built out when you get here — tell Claude Code.

What it will cover:

- `/proc` vs `/sys`: history, philosophy, and when to use which (rule of thumb:
  `/proc` for process/debug info, `/sys` for one-value-per-file device attributes)
- `proc_create()` and the `seq_file` API — printing multi-line reports without
  the buffer-management bugs everyone writes first
- sysfs attributes: `DEVICE_ATTR_RW()`, show/store callbacks, and how this ties
  back to the parameter files you saw in module 03
- kobjects in passing — just enough to know what `/sys` actually is

Planned lab: give `kbuf` a `/proc/kbuf/stats` report (reads, writes, bytes) and
sysfs knobs on its device for capacity and a reset trigger.
