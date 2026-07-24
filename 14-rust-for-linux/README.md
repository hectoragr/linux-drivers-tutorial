# 14 — Rust for Linux (stub)

> 🔜 Built out when you get here — tell Claude Code. Requires module 12's
> QEMU workflow (Rust-for-Linux needs a custom kernel build; your 6.8 distro
> kernel wasn't built with Rust support).

The capstone: the same driver concepts, in the language the kernel community is
adopting for new drivers (Android binder, Apple GPU (asahi), NVMe, PHY drivers
are already in or on the way).

What it will cover:

- Rust in ~a-week-of-evenings *for what kernel work needs*: ownership/borrowing
  (you already think in lifetimes — you've debugged use-after-free in C),
  `Result`/`?` (the goto-unwind ladder from module 04, enforced by the compiler),
  traits (the ops tables from module 01, type-checked)
- Toolchain: rustup, the kernel's pinned rustc, `make LLVM=1 rustavailable`
- Building a Rust-enabled kernel and running it under virtme-ng
- `rust/kernel/` crate tour: `Module`, `miscdevice`, `pr_info!`
- Port `kbuf` to Rust as a misc device — then compare the two side by side:
  where did the mutex go? why can't you forget to unlock it? what happened to
  the error-handling ladder?
- Honest assessment: what's stable, what's churning, where out-of-tree Rust
  modules stand today

This is where "why Rust?" stops being a slogan: entire bug classes you spent
modules 04–11 learning to avoid become compile errors.
