NAME := bsq

HDR := ./src

AR := ar rcs
CC := cc -Wall -Wextra -Werror -I $(HDR) -g
RM := rm -rf
MK := mkdir -p
MAKE := make

SRC_DIR := ./src
OBJ_DIR := ./obj

SRC := \
	bsq.c	\
	utils.c	\
	get_next_line.c	\
	mallocmap.c	\
	norm.c	\

SRC := $(patsubst %, $(SRC_DIR)/%, $(SRC))
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(MK) $(OBJ_DIR)
	@$(CC) -c $< -o $@

$(NAME): $(OBJ)
	@$(CC) $(OBJ) -o $(NAME)

all: $(NAME)

clean:
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

