# 09 — Kernel Memory and mmap (stub)

> 🔜 Built out when you get here — tell Claude Code.

What it will cover:

- The allocator zoo and when to use each: `kmalloc` (physically contiguous,
  small), `vmalloc` (virtually contiguous, large), `kzalloc`, `devm_*` managed
  variants, and the GFP flags (`GFP_KERNEL` vs `GFP_ATOMIC` — the sleep rule
  from module 07 again)
- Pages, `struct page`, and what "physically contiguous" buys you (DMA)
- Implementing `.mmap`: mapping a kernel buffer straight into a process's
  address space — zero-copy I/O
- A taste of the DMA API (`dma_alloc_coherent`) at the concept level

Planned lab: give `kbuf` an `mmap` path, then benchmark `read()` vs `mmap` from
a userspace program and see the copy disappear.
