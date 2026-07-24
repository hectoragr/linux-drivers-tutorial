# 02 — Your First Kernel Module

Goal: build, load, inspect, and unload a kernel module, and understand every line
of both the C file and the Makefile (the Makefile is weirder than the C).

## 2.1 The code

Read `hello.c` — it's short. Key points:

- **`module_init()` / `module_exit()`** register your entry/exit functions.
  Init runs at `insmod`; exit at `rmmod`. If init returns nonzero, the load
  fails and exit is *never* called.
- **`printk(KERN_INFO ...)` / `pr_info()`** — the kernel's printf. It goes to the
  kernel ring buffer (`dmesg`), not your terminal. `pr_info()`, `pr_err()`, etc.
  are just `printk` with the level baked in — use those.
- **`__init` / `__exit`** — section annotations. `__init` code is *freed from
  memory* after init completes. On a microcontroller you'd recognize this as
  linker-section games; that's exactly what it is (see the kernel's linker
  script if curious).
- **`MODULE_LICENSE("GPL")`** — not decoration. A non-GPL license *taints* the
  kernel and locks you out of most exported symbols. Omit it and the build warns;
  many symbols simply won't link.

## 2.2 The Makefile — kbuild

Out-of-tree modules don't get compiled by your Makefile directly. Your Makefile
*re-invokes the kernel's own build system* (kbuild), pointing it back at this
directory:

```make
obj-m += hello.o                     # "build hello.o into a module"

make -C /lib/modules/$(uname -r)/build M=$(PWD) modules
```

- `-C ...build` — cd into the kernel headers tree (it has the real Makefile)
- `M=$(PWD)` — "the module source lives over there"
- kbuild then compiles with the *kernel's* flags, the *kernel's* config, and
  generates the module metadata (`.mod.c`, vermagic) that makes the `.ko` loadable
  on exactly this kernel.

This is why the `obj-m` line and the `make -C` line live in the same file but
are read by *different* make invocations. Look at `Makefile` — it separates the
two roles with a conditional you'll see in most real out-of-tree drivers.

## 2.3 Build, load, inspect, unload

Terminal 2 (leave it running): `sudo dmesg -w`

Terminal 1:

```bash
make                        # produces hello.ko
modinfo hello.ko            # metadata: license, description, vermagic
sudo insmod hello.ko        # watch terminal 2!
lsmod | grep hello          # it's resident
cat /proc/modules | grep hello
ls /sys/module/hello/       # every module gets a sysfs directory
sudo rmmod hello            # watch terminal 2 again
make clean
```

Things worth noticing:

- `modinfo` shows `vermagic: 6.8.0-136-generic ...` — the load-time compatibility
  check from module 00.
- `/sys/module/hello/refcnt` — the reference count; `rmmod` refuses if it's != 0.
- Try `insmod` twice: `File exists`. Modules are singletons.

## 2.4 What actually happened at insmod?

1. `insmod` read the `.ko` (an ELF relocatable) and passed it to the
   `finit_module()` syscall.
2. The kernel verified vermagic + symbol CRCs, allocated kernel memory, copied
   the sections in, and **resolved your undefined symbols** (`printk`...) against
   its exported symbol table — a runtime linker.
3. It ran your init function. Return 0 → module live; return -E... → everything
   is unwound.

## Exercises

1. Return `-EIO` from `hello_init` (add `#include <linux/errno.h>`). Build, insmod.
   What does insmod print? Is the module in `lsmod`? Does the exit message ever
   appear in dmesg?
2. Remove the `MODULE_LICENSE` line and rebuild. Read the warning carefully.
   Then set it to `"Proprietary"`, load it, and check `cat /proc/sys/kernel/tainted`
   before and after. (Put it back to GPL after. The taint flag sticks until reboot.)
3. Add a `pr_debug()` call. Load the module — why doesn't it appear in dmesg?
   (Teaser for module 11: dynamic debug.)
4. Find `hello`'s init message in `journalctl -k` as well — know both tools.

## Checkpoint

- You can explain why the Makefile invokes `make -C ... M=$PWD`.
- You know what happens to a module whose init returns an error.
- You know where `printk` output goes and how to watch it live.

Next: [03-module-parameters](../03-module-parameters/).
