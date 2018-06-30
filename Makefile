##
## Makefile
##
## Licensed under MIT license
##

.PHONY: all clean fclean re

SRC_DIR := ./sources
HDR_DIR := ./headers
BIN_DIR := ./bin
OBJ_DIR := $(BIN_DIR)/obj

SRCS := $(shell find $(SRC_DIR) -name "*.c")
OBJS := $(SRCS:.c=.o)
NAME := ammonium

WARNINGS := -Wall -Wextra -Werror -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wstrict-prototypes
CFLAGS := -std=c89 -s -O2 -I$(HDR_DIR) $(WARNINGS) -o $(BIN_DIR)/$(NAME)-$(shell date --iso=seconds).bin -lm

all: $(OBJS)
	mv $(OBJS) $(OBJ_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/*.o

clean:
	$(RM) $(OBJ_DIR)/*.o

fclean: clean
	$(RM) $(BIN_DIR)/*.bin

re: fclean all
