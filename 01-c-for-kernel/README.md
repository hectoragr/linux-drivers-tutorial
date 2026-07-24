# 01 — C, Through the Kernel's Eyes

You already know C from microcontrollers. This module isn't "what is a pointer" —
it's the four C idioms the kernel is *built* on, each as a small userspace program
you can compile and step through with regular tools (gcc, gdb) before we lose those
comforts in kernel space.

Build everything:

```bash
make        # builds all exercises into ./bin/
make run    # builds and runs them all
```

## 1.1 Ops tables — polymorphism in C (`ex1_ops_table.c`)

The kernel is object-oriented C. A "driver" is essentially a struct of function
pointers (an *ops table*) registered with a subsystem. When you `read()` a file,
the VFS calls `file->f_op->read(...)` — dynamic dispatch, no C++ required.

`ex1_ops_table.c` builds a miniature version: two "devices" (a UART and a flash
chip, to keep it close to home) implementing the same `struct dev_ops` interface,
and generic code that drives either through the table. This exact pattern is
`struct file_operations` in module 04.

Things to notice in the code:
- The ops struct holds `const` function pointers and is itself declared `static const` —
  kernel convention, and it lands the table in read-only memory.
- "Private data" travels as a `void *` alongside the ops — the kernel does this
  everywhere (`file->private_data`).

## 1.2 `offsetof` and `container_of` (`ex2_container_of.c`)

The single most important macro in the kernel. Given a pointer to a *member* of a
struct, `container_of` recovers a pointer to the *whole struct*:

```c
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
```

Why it matters: kernel subsystems hand you back pointers to *generic* embedded
structures (a `struct cdev`, a `list_head`, a `work_struct`), and you use
`container_of` to get back to *your* device struct that embeds them. It's
upcasting/downcasting, done with pointer arithmetic.

`ex2_container_of.c` implements the macro from scratch and proves to you, with
printed addresses, exactly what the arithmetic does.

## 1.3 Intrusive linked lists (`ex3_list.c`)

On a microcontroller you probably wrote lists where the node *contains* the data.
The kernel inverts this: the data contains the node. Every kernel object that can
be on a list embeds a `struct list_head { struct list_head *next, *prev; }`, and
one generic set of list functions manipulates all of them — with `container_of`
recovering the owner.

`ex3_list.c` reimplements the kernel's `list_head` API (a working subset:
`LIST_HEAD`, `list_add_tail`, `list_del`, `list_for_each_entry`) and uses it to
manage a list of fake devices. After this, `include/linux/list.h` in the real
kernel will read like your own code.

## 1.4 Bits, registers, and `volatile` (`ex4_bits.c`)

Home turf for you — but with the kernel's vocabulary: `BIT(n)`, `GENMASK(h, l)`,
field extraction, read-modify-write. The exercise simulates a memory-mapped
control register and manipulates it with the macros drivers actually use.

One kernel-specific point the file demonstrates: why `volatile` on a variable is
almost always wrong in kernel code (the kernel uses `READ_ONCE`/`WRITE_ONCE` and
proper accessors like `readl`/`writel` instead — precision tools where `volatile`
is a sledgehammer).

## Exercises

1. **ex1:** Add a third device type (`spi_eeprom`) with its own ops implementation,
   without touching any of the generic driver code. That's the whole point of ops
   tables — feel how the seam works.
2. **ex2:** Add a second embedded member to `struct my_device` and recover the
   container from a pointer to *that*. Predict the offsets on paper first.
3. **ex3:** Implement `list_for_each_entry_safe` (needed to delete nodes while
   iterating — look at the real one in `/usr/src/linux-hwe-6.8-headers-6.8.0-136/include/linux/list.h`
   if stuck) and use it to free the entire list.
4. **ex4:** Write `FIELD_PREP`/`FIELD_GET`-style macros that insert/extract a
   value into/from a mask, and use them to set a 3-bit "baud rate" field without
   disturbing neighboring bits.

## Checkpoint

You're ready for real kernel code when:
- You can write `container_of` from memory and explain the cast dance.
- Embedding `struct list_head` inside your own struct feels natural, not backwards.
- You understand why the generic code in ex1/ex3 never needs to know your types.

Next: [02-hello-module](../02-hello-module/) — into the kernel for real.
