# 10 — The Driver Model: Platform Drivers & Device Tree (stub)

> 🔜 Built out when you get here — tell Claude Code.

Everything so far created devices out of thin air in `module_init`. Real drivers
don't do that: the *bus* discovers hardware and the driver model *matches* it to
a driver, calling your `probe()`. This module is the bridge from "toy modules"
to how `snd-hda-codec-cs8409` and every driver on your machine actually work.

What it will cover:

- The device/driver/bus triangle; `probe()`/`remove()` as the real entry points
- `platform_device` and `platform_driver` — the bus for non-discoverable hardware
- Device Tree: how embedded Linux describes hardware (`compatible` strings,
  `of_match_table`) — essential for your electronics background; this is where
  schematics meet software
- ACPI matching on x86 in passing (your laptop's world)
- Resource management done right: `devm_*` APIs and why probe/remove balance

Planned lab: a self-contained pair — one module registers a fake
`platform_device` (standing in for a bootloader/DT), another provides the
`platform_driver` that probes it, maps its "registers", and exposes them.
