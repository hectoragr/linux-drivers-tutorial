/*
 * ex3_list.c — intrusive doubly-linked lists, kernel style.
 *
 * A working subset of include/linux/list.h. The trick: the NODE lives
 * inside YOUR struct (intrusive), and container_of recovers the owner.
 * One implementation serves every list in the kernel.
 */
#include <stdio.h>
#include <stddef.h>

#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))

/* ---- mini list.h -------------------------------------------------------- */

struct list_head {
	struct list_head *next, *prev;
};

/* A list head starts pointing at itself: an empty circular list. */
#define LIST_HEAD(name) struct list_head name = { &(name), &(name) }

static void __list_add(struct list_head *new,
		       struct list_head *prev, struct list_head *next)
{
	next->prev = new;
	new->next  = next;
	new->prev  = prev;
	prev->next = new;
}

static void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static void list_del(struct list_head *entry)
{
	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
	entry->next = entry->prev = NULL; /* poison: catch use-after-del */
}

static int list_empty(const struct list_head *head)
{
	return head->next == head;
}

/* Get the owning struct of a node. */
#define list_entry(ptr, type, member) container_of(ptr, type, member)

/* Iterate over owners, not nodes. This is the form you'll use daily. */
#define list_for_each_entry(pos, head, member)                            \
	for (pos = list_entry((head)->next, __typeof__(*pos), member);    \
	     &pos->member != (head);                                       \
	     pos = list_entry(pos->member.next, __typeof__(*pos), member))

/* ---- using it ----------------------------------------------------------- */

struct sensor {
	const char       *name;
	int               last_reading;
	struct list_head  node;   /* the intrusive hook */
};

int main(void)
{
	LIST_HEAD(sensors);       /* empty list */

	struct sensor temp  = { .name = "tmp117",  .last_reading = 231 };
	struct sensor accel = { .name = "lis3dh",  .last_reading = -12 };
	struct sensor mag   = { .name = "ak8963",  .last_reading = 55  };
	struct sensor *s;

	list_add_tail(&temp.node,  &sensors);
	list_add_tail(&accel.node, &sensors);
	list_add_tail(&mag.node,   &sensors);

	printf("registered sensors:\n");
	list_for_each_entry(s, &sensors, node)
		printf("  %-8s reading=%d (struct at %p, node at %p)\n",
		       s->name, s->last_reading, (void *)s, (void *)&s->node);

	printf("\nremoving 'lis3dh'...\n");
	list_del(&accel.node);

	list_for_each_entry(s, &sensors, node)
		printf("  %-8s reading=%d\n", s->name, s->last_reading);

	printf("\nlist_empty: %s\n", list_empty(&sensors) ? "yes" : "no");

	/*
	 * EXERCISE: implement list_for_each_entry_safe(pos, n, head, member)
	 * — it must survive list_del(&pos->member) inside the loop body.
	 * Then delete every remaining node in one pass.
	 */
	return 0;
}
