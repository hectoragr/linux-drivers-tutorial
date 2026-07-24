# 08 — Interrupts and Deferred Work (stub)

> 🔜 Built out when you get here — tell Claude Code.

What it will cover:

- `request_irq()`, IRQ handlers, and the top-half/bottom-half split (you know
  "keep the ISR short" from microcontrollers — here it's enforced by design)
- Threaded IRQs — the modern default
- Deferral mechanisms and when to use each: softirqs (context), tasklets
  (legacy, being removed), **workqueues** (your workhorse), and timers
  (`timer_list`, `hrtimer`)
- Getting a real IRQ without hardware: hooking a shared interrupt, and/or using
  the i8042 keyboard controller as a lab rat

Planned lab: a module that timestamps keyboard interrupts and streams the
inter-arrival times out through a char device (everything from 04–07 combined).
