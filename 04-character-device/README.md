# 04 — Character Devices: Your Driver Becomes a File

Goal: build `kbuf`, a driver that creates `/dev/kbuf0` — a 4 KiB buffer living in
kernel memory that you can `echo` into and `cat` out of. This module is the heart
of Part 1: it connects *everything* — ops tables, `container_of`, and the
"everything is a file" design — into one working driver.

## 4.1 The theory: how `cat /dev/kbuf0` reaches your code

```
cat /dev/kbuf0
 └─ read(fd, ...)                        userspace
 ────────────────────────────────────── syscall boundary
 └─ VFS: file->f_op->read(...)           generic kernel
     └─ kbuf_read(...)                   YOUR CODE
```

The plumbing that makes the connection, in load order:

1. **`dev_t` — device numbers.** A (major, minor) pair. Major ≈ "which driver",
   minor ≈ "which device instance". `alloc_chrdev_region()` asks the kernel to
   pick a free major dynamically (hardcoding majors is 1990s practice).
   See the current map: `cat /proc/devices`.
2. **`struct cdev`** — the kernel object that says "this dev_t range is handled
   by this `file_operations` table". We *embed* it in our device struct
   (module 01, exercise 2 — this is why you practiced `container_of`).
3. **`struct file_operations`** — the ops table (module 01, ex1, for real now):
   `.open`, `.read`, `.write`, `.llseek`, `.release`.
4. **`class_create` + `device_create`** — registers the device in `/sys/class/`,
   which emits a *uevent*; udev hears it and creates `/dev/kbuf0` for us.
   (Old tutorials say `mknod` — that still works and exercise 1 makes you do it
   once, so you know what udev automates.)

## 4.2 The two rules of kernel↔user data transfer

Your `read`/`write` handlers receive a userspace pointer (`char __user *buf`).
**You must never dereference it directly.** Two reasons:

- It may be invalid/hostile — a bad userspace pointer must yield `-EFAULT` for
  the *caller*, not an oops for the *kernel*.
- The `__user` annotation is checked by `sparse` (module 11); the compiler alone
  won't stop you.

So all traffic goes through checked copiers:

```c
copy_to_user(user_buf, kernel_buf, n);    /* read path  */
copy_from_user(kernel_buf, user_buf, n);  /* write path */
```

Both return the number of bytes that could NOT be copied (0 = success).

Second rule: **read/write handlers do partial I/O.** You get an offset pointer
`*ppos`; return however many bytes you handled and advance `*ppos`. Returning 0
from read means EOF — that's how `cat` knows to stop. Get this protocol wrong and
`cat` hangs forever or spins at 100% CPU (exercise 3 makes you experience it).

## 4.3 Walk through `kbuf.c`

Read it top to bottom before building — it's ~150 lines, heavily commented.
The flow to trace:

- `kbuf_init`: region → cdev → class → device. Note the error-handling ladder of
  `goto` labels that unwinds in reverse order — this is *the* canonical kernel
  error-handling idiom, not spaghetti.
- `kbuf_open`: `container_of(inode->i_cdev, ...)` recovers our device struct and
  parks it in `file->private_data` — from then on every handler has cheap access.
- `kbuf_read`/`kbuf_write`: offset math first, then `copy_*_user`, then advance
  `*ppos`.
- There's a `struct mutex` guarding the buffer. Two shells writing at once would
  otherwise race. Take it on faith here; module 07 is entirely about this.

## 4.4 Build and play

```bash
make
sudo insmod kbuf.ko
sudo dmesg | tail -3            # note the major number it got
ls -l /dev/kbuf0                # udev made this — note major:minor match

# it's a file!
echo "hello from userspace" | sudo tee /dev/kbuf0
sudo cat /dev/kbuf0
sudo dd if=/dev/kbuf0 bs=5 count=1 2>/dev/null   # partial reads work
echo "x" | sudo dd of=/dev/kbuf0 seek=100 bs=1 2>/dev/null  # sparse-ish writes

cat /sys/class/kbuf/kbuf0/dev   # the sysfs side: "major:minor"
cat /proc/devices | grep kbuf   # your dynamic major, registered

sudo rmmod kbuf
ls /dev/kbuf0                   # gone — udev cleans up too
```

Or run the whole thing: `sudo ./test.sh` (build + load + exercise + unload, with
pass/fail output).

## Exercises

1. **mknod by hand:** while loaded, `sudo mknod /dev/kbuf-manual c <major> 0`
   and use it. Same driver, second door. Delete it; explain why udev's way wins.
2. **More minors:** the driver reserves 1 minor. Make it create `/dev/kbuf0` and
   `/dev/kbuf1` with *independent* buffers (hint: array of `struct kbuf_dev`,
   loop in init, minor = index. `container_of` in open already does the right
   thing — understand why).
3. **Break the protocol on purpose:** make `kbuf_read` never update `*ppos` and
   always return `count`. Run `cat /dev/kbuf0`. Ctrl-C, revert, and write one
   sentence on why `cat` behaved as it did.
4. **Size cap:** writes past 4 KiB currently return `-ENOSPC`. Change the policy
   to "overwrite oldest data" (ring buffer). Sketch the index math on paper first
   — it's the same as a UART FIFO you've written on bare metal.

## Checkpoint

- You can draw the path from `cat` to `kbuf_read` from memory.
- You know why `copy_to_user` exists and what its return value means.
- You can explain the goto-unwind pattern in `kbuf_init` and why the order in
  `kbuf_exit` is its mirror image.

Next: 05-ioctl-and-misc — commands that aren't reads or writes, and the
`miscdevice` shortcut that shrinks this module's boilerplate by half.
