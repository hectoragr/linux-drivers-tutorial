# 07 — Concurrency: the Hard Part (stub)

> 🔜 Built out when you get here — tell Claude Code.

The kernel is a preemptive, multi-core environment: your code runs simultaneously
on several CPUs, gets preempted mid-line, and shares data with interrupt context.
Coming from single-core microcontrollers with "disable interrupts" as the only
lock, this is the module that most changes how you think.

What it will cover:

- Race conditions you can actually reproduce (a deliberately broken counter
  module + a userspace hammer script)
- `atomic_t` and when it's enough
- Spinlocks vs mutexes: the sleep/no-sleep rule, and *context* (can this code
  sleep? am I in an interrupt?) as the question behind every choice
- Wait queues and blocking reads: making `cat /dev/...` sleep until data arrives
  (producer/consumer, the kernel way)
- A first look at lockdep — the kernel's built-in deadlock detector

Planned lab: make `kbuf` a proper blocking FIFO — readers sleep on empty,
writers wake them — verified under concurrent load.
