# 11 — Debugging and Kernel Tooling (stub)

> 🔜 Built out when you get here — tell Claude Code.

What it will cover:

- Reading an oops: the stack trace, `RIP`, decoding with `./scripts/decode_stacktrace.sh`
  (we'll crash a sacrificial module on purpose — in the module 12 VM)
- Dynamic debug: turning `pr_debug()` on per-file/per-line at runtime
  (`/sys/kernel/debug/dynamic_debug/control`) — resolves the module 02 teaser
- ftrace and `trace-cmd`: watching your driver's function calls live
- Static analysis: `sparse` (those `__user` annotations), `checkpatch.pl`
  (kernel style), and W=1 builds
- `debugfs` — a scratchpad filesystem for driver internals
- KASAN/lockdep existence and what they catch

Planned lab: instrument and debug a module with three planted bugs (a leak, a
race, a use-after-free) using the tools above.
