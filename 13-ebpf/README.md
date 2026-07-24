# 13 — eBPF: Kernel Programming Without Modules (stub)

> 🔜 Built out when you get here — tell Claude Code.

The most significant change in kernel development since you last looked at it.
eBPF lets you load *verified, sandboxed* programs into the running kernel — no
module, no taint, no crash risk. It has eaten large parts of tracing, networking,
and security tooling.

What it will cover:

- The mental model: bytecode + in-kernel verifier + JIT; why it can't crash the
  kernel (and what it therefore can't do — contrast with everything you've built)
- `bpftrace` one-liners: trace *your own kbuf driver's* functions with kprobes —
  instant gratification
- kprobes/tracepoints vs the module-based instrumentation from module 11
- A minimal libbpf + CO-RE C program: map + user-space reader
- Where eBPF genuinely replaces driver work (observability, XDP) and where it
  doesn't (actual hardware drivers)

Planned lab: attach kprobes to `kbuf_read`/`kbuf_write` and build a live
histogram of I/O sizes — zero changes to the driver itself.
