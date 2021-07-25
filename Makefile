##
## Makefile
##
## Licensed under MIT license
##


CC ?= gcc

NAME := senpai

WARNINGS := -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wstrict-prototypes
CFLAGS ?= -O2 -g3
CFLAGS := $(CFLAGS) -std=c99 -U__STRICT_ANSI__ -I./headers $(WARNINGS)
LDLIBS := -lm

SRCS := $(wildcard sources/*.c)
OBJS := $(SRCS:.c=.o)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) -rf $(OBJS) $(NAME)

.PHONY: clean
