CC ?= gcc
AR ?= ar
CFLAGS ?= -O3 -Wall -Wextra -Iinclude -pthread
ARFLAGS = rcs

SRCS = src/core.c src/parser.c src/worker.c src/rules/syntax.c src/rules/security.c src/rules/formatting.c
OBJS = $(SRCS:.c=.o)
LIB = libasmlint.a
BIN = asmlint

all: $(LIB) $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(BIN): cli/main.c $(LIB)
	$(CC) $(CFLAGS) cli/main.c -L. -lasmlint -o $@

clean:
	rm -f $(OBJS) $(LIB) $(BIN)

.PHONY: all clean
