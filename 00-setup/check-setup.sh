#!/usr/bin/env bash
# Verify the toolchain needed for this tutorial.
set -u

ok=0; fail=0
pass() { echo "  ✅ $1"; ok=$((ok+1)); }
warn() { echo "  ⚠️  $1"; }
err()  { echo "  ❌ $1"; fail=$((fail+1)); }

echo "== Toolchain =="
command -v gcc  >/dev/null && pass "gcc:  $(gcc --version | head -1)"  || err "gcc missing (sudo apt install build-essential)"
command -v make >/dev/null && pass "make: $(make --version | head -1)" || err "make missing (sudo apt install build-essential)"

echo "== Kernel headers =="
KREL=$(uname -r)
if [ -e "/lib/modules/$KREL/build/Makefile" ]; then
    pass "headers for running kernel $KREL found"
else
    err "no headers for $KREL (sudo apt install linux-headers-$KREL)"
fi

echo "== Module tools =="
for t in insmod rmmod modinfo lsmod dmesg; do
    command -v "$t" >/dev/null && pass "$t" || err "$t missing (sudo apt install kmod)"
done

echo "== Optional (needed in later modules) =="
command -v qemu-system-x86_64 >/dev/null && pass "qemu (module 12)" || warn "qemu not installed — needed in module 12"
command -v bpftrace >/dev/null && pass "bpftrace (module 13)"       || warn "bpftrace not installed — needed in module 13"
command -v rustc >/dev/null    && pass "rust (module 14)"           || warn "rust not installed — needed in module 14"

echo
[ "$fail" -eq 0 ] && echo "All required checks passed ($ok ok). You're good to go." \
                  || echo "$fail required check(s) failed — fix those before module 02."
exit "$fail"
