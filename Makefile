##
## Makefile
##
## Licensed under MIT license
##


CC ?= gcc

NAME := senpai
TEST_NAME := test_senpai

WARNINGS := -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wstrict-prototypes
CFLAGS ?= -O2 -g3 -fopenmp
CFLAGS := $(CFLAGS) -std=c99 -U__STRICT_ANSI__ -I./headers $(WARNINGS)
LDLIBS := -lm -fopenmp
TEST_LDLIBS := -lm -fopenmp -lcriterion

DEPFILES := $(wildcard sources/*.d)
SRCS := $(wildcard sources/*.c)
OBJS := $(SRCS:.c=.o)

TEST_SRCS := $(wildcard tests/*.c)
TEST_SRCS += $(filter-out sources/main.c, $(wildcard sources/*.c))
TEST_OBJS := $(TEST_SRCS:.c=.o)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(TEST_NAME): $(TEST_OBJS)
	$(CC) $(LDFLAGS) $^ $(TEST_LDLIBS) -o $@

%.o: %.c
	$(CC) -MMD -MP -MF $*.d $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

test: $(TEST_NAME)
	./$(TEST_NAME) --verbose

clean:
	$(RM) -rf $(DEPFILES) $(OBJS) $(NAME) $(TEST_OBJS) $(TEST_NAME)

install:
	cp senpai /usr/bin/senpai

-include $(DEPFILES)

.PHONY: clean install
