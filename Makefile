##
## Makefile
##
## Licensed under MIT license
##

.PHONY: all clean fclean re

SRC_DIR := ./sources
HDR_DIR := ./headers

SRCS := $(shell find $(SRC_DIR) -name "*.c")
OBJS := $(shell find $(SRC_DIR) -name "*.o")
NAME := ammonium

WARNINGS := -Wall -Wextra -Werror -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wstrict-prototypes
CFLAGS := -std=c89 -lm -s -O2 -I$(HDR_DIR) $(WARNINGS) -o $(NAME).bin

all:
	gcc $(SRCS) $(CFLAGS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME).bin

re: fclean all
