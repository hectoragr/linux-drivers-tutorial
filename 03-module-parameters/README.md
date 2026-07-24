# 03 — Module Parameters

Goal: make a module configurable at load time, and meet sysfs for the first time.

Real-world hook: your `snd-hda-codec-cs8409` audio driver takes parameters this
way, and so does almost every driver: `modinfo snd_hda_intel | grep parm` shows a
couple dozen.

## 3.1 The API

```c
static int  rate  = 100;
static bool verbose = false;
static char *label = "default";
static int  thresholds[4] = { 10, 20, 30, 40 };
static int  n_thresholds  = 4;

module_param(rate, int, 0644);
module_param(verbose, bool, 0644);
module_param(label, charp, 0444);
module_param_array(thresholds, int, &n_thresholds, 0444);
MODULE_PARM_DESC(rate, "Sampling rate in Hz");
```

Three arguments: the variable, its *type as a token* (`int`, `bool`, `charp`,
`uint`, ...), and **sysfs permissions** in octal:

- `0444` — world-readable in `/sys/module/<name>/parameters/`, fixed after load
- `0644` — root can *change it at runtime* by writing the sysfs file
- `0` — no sysfs entry at all; load-time only

That third one is the interesting bit: a writable parameter means your variable
can change under you at any moment, from outside your code. (Foreshadowing
module 07 — nothing synchronizes that write with your readers.)

## 3.2 Build and experiment

```bash
make
modinfo params.ko          # note the parm: lines — MODULE_PARM_DESC output

# defaults
sudo insmod params.ko
sudo dmesg | tail -8
sudo rmmod params

# override at load time
sudo insmod params.ko rate=48000 verbose=1 label="bench-psu" thresholds=1,2,3
sudo dmesg | tail -8

# inspect via sysfs
ls -l /sys/module/params/parameters/
cat /sys/module/params/parameters/rate

# change a 0644 param live — no reload!
echo 96000 | sudo tee /sys/module/params/parameters/rate
cat /sys/module/params/parameters/rate

sudo rmmod params
sudo dmesg | tail -4       # exit handler prints final values — was rate 96000?
```

Note what you did *not* get: no notification when `rate` changed. The variable
just changed. If your driver needed to react (reprogram a PLL, say), plain
`module_param` isn't enough — that's exercise 3.

Also try passing garbage: `sudo insmod params.ko rate=fast` — kbuild's type
system rejects it at load, not at use.

## 3.3 How modprobe fits in

`insmod params.ko rate=48000` is manual. Installed drivers get their options from
`/etc/modprobe.d/*.conf`:

```
options snd-hda-intel model=mbp143
```

That's how your MacBook audio quirks are configured system-wide. `modprobe` reads
those files; raw `insmod` does not.

## Exercises

1. Add an `unsigned int` parameter with perm `0` and confirm it's absent from
   `/sys/module/params/parameters/`.
2. `thresholds=1,2,3` loads 3 values into an array sized 4. Print `n_thresholds`
   in init — what is it? What happens with `thresholds=1,2,3,4,5`?
3. Advanced: replace `module_param(rate, ...)` with `module_param_cb()` and a
   `struct kernel_param_ops` whose `.set` callback validates the new value
   (reject anything above 192000) and logs the change. This is the ops-table
   pattern from module 01 — your first real use of it in the kernel.
4. Look at a real example: `grep -rn "module_param" ~/snd-hda-codec-cs8409/ | head`.

## Checkpoint

- You know the three ways a parameter gets set (insmod args, modprobe.d, sysfs).
- You can explain what the octal permission argument controls.
- You've seen `/sys/module/` and it's starting to look like an API, not magic.

Next: [04-character-device](../04-character-device/) — the big one: your driver
becomes a file.
