##
## Makefile
##
## Licensed under MIT license
##

.PHONY: all clean fclean re

$(CC) := gcc

SRC_DIR := ./sources
HDR_DIR := ./headers
BUILD_DIR := ./build

NAME := senpai

WARNINGS := -Wall -Wextra -Werror -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wstrict-prototypes
OPTIONS := -std=c99 -O2 -g3 -U__STRICT_ANSI__
LIBS := -lm
CFLAGS := -I$(HDR_DIR) $(WARNINGS) $(OPTIONS) $(LIBS)

SRCS := $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJS := $(addprefix $(BUILD_DIR),$(patsubst %.c,%.o,$(SRCS)))


$(OBJS): $(SRCS)
	$(CC) -c $(CFLAGS) $< -o $@

all:
	$(CC) $(OBJS) $(CFLAGS) -o $(NAME)

clean:
	$(RM) -rf $(BUILD_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all
