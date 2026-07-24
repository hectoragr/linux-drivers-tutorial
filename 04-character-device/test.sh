#!/usr/bin/env bash
# Build, load, exercise, and unload the kbuf driver.
# Run with: sudo ./test.sh
set -u

if [ "$(id -u)" -ne 0 ]; then
    echo "run as root: sudo ./test.sh"; exit 1
fi

pass=0; fail=0
check() { # check <description> <expected> <actual>
    if [ "$2" = "$3" ]; then
        echo "  ✅ $1"; pass=$((pass+1))
    else
        echo "  ❌ $1: expected '$2', got '$3'"; fail=$((fail+1))
    fi
}

cd "$(dirname "$0")"

echo "== build =="
make -s || { echo "build failed"; exit 1; }

echo "== load =="
rmmod kbuf 2>/dev/null || true
insmod kbuf.ko || { echo "insmod failed"; dmesg | tail -5; exit 1; }
sleep 0.3   # give udev a beat to create the node

[ -c /dev/kbuf0 ] && check "/dev/kbuf0 exists" "yes" "yes" \
                  || check "/dev/kbuf0 exists" "yes" "no"

echo "== read/write =="
echo -n "hola kernel" > /dev/kbuf0
check "read back what we wrote" "hola kernel" "$(cat /dev/kbuf0)"

check "partial read (dd bs=4 count=1)" "hola" \
      "$(dd if=/dev/kbuf0 bs=4 count=1 2>/dev/null)"

echo -n "HOLA" > /dev/kbuf0    # fresh open: offset 0 overwrites
check "overwrite from offset 0" "HOLA kernel" "$(cat /dev/kbuf0)"

echo "== EOF protocol =="
# If read never returned 0, this cat would hang forever (see exercise 3).
timeout 2 cat /dev/kbuf0 >/dev/null \
      && check "cat terminates (EOF works)" "yes" "yes" \
      || check "cat terminates (EOF works)" "yes" "no"

echo "== unload =="
rmmod kbuf
sleep 0.3
[ -e /dev/kbuf0 ] && check "/dev/kbuf0 removed" "yes" "no" \
                  || check "/dev/kbuf0 removed" "yes" "yes"

echo
echo "$pass passed, $fail failed"
dmesg | tail -4
exit "$fail"
