##
## Makefile
##
## Licensed under MIT license
##

.PHONY: all clean fclean re

$(CC) := mpicc

SRC_DIR := ./sources
HDR_DIR := ./headers

SRCS := $(shell find $(SRC_DIR) -name "*.c")
OBJS := $(shell find $(SRC_DIR) -name "*.o")
NAME := senpai

LIBS := -lm
CFLAGS := -I$(HDR_DIR) $(WARNINGS) $(OPTIONS) $(LIBS) -o $(NAME).bin

all:
	$(CC) $(SRCS) $(CFLAGS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME).bin

re: fclean all
