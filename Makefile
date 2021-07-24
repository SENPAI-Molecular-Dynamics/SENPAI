##
## Makefile
##
## Licensed under MIT license
##

.PHONY: all clean fclean re info

$(CC) := gcc

SRC_DIR := ./sources
HDR_DIR := ./headers
BUILD_DIR := ./build

NAME := senpai

WARNINGS := -Wall -Wextra -Werror -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wstrict-prototypes
OPTIONS := -std=c99 -O2 -g3 -U__STRICT_ANSI__
LIBS := -lm
CFLAGS := -I$(HDR_DIR) $(WARNINGS) $(OPTIONS) $(LIBS)

SRCS := $(wildcard $(SRC_DIR)/*.c)
SRC_NAMES := $(notdir $(SRCS))
OBJ_NAMES := $(SRC_NAMES:.c=.o)
OBJS := $(addprefix $(BUILD_DIR)/,$(OBJ_NAMES))

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(NAME).bin: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(BUILD_DIR)/$(NAME).bin

all: $(BUILD_DIR)/$(NAME).bin
	cp $(BUILD_DIR)/$(NAME).bin ./$(NAME)

clean:
	$(RM) -rf $(BUILD_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all
