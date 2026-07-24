CC      := gcc
CFLAGS  := -Wall -Wextra -g -O1
BIN     := bin
SRCS    := $(wildcard ex*.c)
PROGS   := $(SRCS:%.c=$(BIN)/%)

all: $(PROGS)

$(BIN)/%: %.c | $(BIN)
	$(CC) $(CFLAGS) -o $@ $<

$(BIN):
	mkdir -p $(BIN)

run: all
	@for p in $(PROGS); do echo "=== $$p ==="; ./$$p; echo; done

clean:
	rm -rf $(BIN)

.PHONY: all run clean
