/*
 * ex4_bits.c — bit manipulation with the kernel's vocabulary.
 *
 * You've done register twiddling on microcontrollers; the kernel just
 * has nicer names for it: BIT(), GENMASK(), and field helpers.
 */
#include <stdio.h>
#include <stdint.h>

#define BIT(n)         (1UL << (n))
/* Contiguous mask from bit h down to bit l, inclusive.
 * GENMASK(6, 4) = 0b0111'0000 */
#define GENMASK(h, l)  (((~0UL) << (l)) & (~0UL >> (63 - (h))))

/* A made-up UART control register layout:
 *  bit  0     : ENABLE
 *  bit  1     : LOOPBACK
 *  bits 6..4  : BAUD_SEL (0..7)
 *  bit  31    : RESET (self-clearing)
 */
#define CTRL_ENABLE       BIT(0)
#define CTRL_LOOPBACK     BIT(1)
#define CTRL_BAUD_MASK    GENMASK(6, 4)
#define CTRL_BAUD_SHIFT   4
#define CTRL_RESET        BIT(31)

static void dump(const char *what, uint32_t reg)
{
	printf("%-28s = 0x%08x  (EN=%lu LOOP=%lu BAUD_SEL=%lu)\n",
	       what, reg,
	       (reg & CTRL_ENABLE)   ? 1UL : 0UL,
	       (reg & CTRL_LOOPBACK) ? 1UL : 0UL,
	       (unsigned long)((reg & CTRL_BAUD_MASK) >> CTRL_BAUD_SHIFT));
}

int main(void)
{
	/* Simulated memory-mapped register. In a real driver this would be
	 * ioremap()'d and accessed with readl()/writel() — NEVER a bare
	 * volatile pointer dereference. */
	uint32_t ctrl = 0;

	dump("power-on", ctrl);

	/* Set: read-modify-write with OR */
	ctrl |= CTRL_ENABLE | CTRL_LOOPBACK;
	dump("enable + loopback", ctrl);

	/* Field update: clear the field, then OR in the new value.
	 * Forgetting the clear step is the classic bug. */
	ctrl = (ctrl & ~CTRL_BAUD_MASK) | (5UL << CTRL_BAUD_SHIFT);
	dump("baud_sel := 5", ctrl);

	ctrl = (ctrl & ~CTRL_BAUD_MASK) | (2UL << CTRL_BAUD_SHIFT);
	dump("baud_sel := 2 (5 gone?)", ctrl);

	/* Clear: AND with complement */
	ctrl &= ~CTRL_LOOPBACK;
	dump("loopback off", ctrl);

	/* Test */
	printf("\nenabled? %s\n", (ctrl & CTRL_ENABLE) ? "yes" : "no");

	/*
	 * Why not `volatile uint32_t *reg` in kernel code?
	 * - volatile forbids the compiler from caching/reordering THAT access,
	 *   but says nothing about CPU reordering, write buffers, or bus
	 *   posting — the things that actually bite on real hardware.
	 * - The kernel's readl()/writel() include the needed memory barriers
	 *   and correct-width accesses; READ_ONCE()/WRITE_ONCE() handle the
	 *   compiler-only cases. Each tool is explicit about what it solves.
	 *
	 * EXERCISE: write FIELD_PREP(mask, val) and FIELD_GET(mask, reg)
	 * macros that derive the shift from the mask itself (hint: how do
	 * you find the lowest set bit of CTRL_BAUD_MASK?), then redo the
	 * baud updates with them.
	 */
	return 0;
}
