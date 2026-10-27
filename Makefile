# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rdomange <romitdomange@gmail.com>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/08/03 14:31:11 by rdomange          #+#    #+#              #
#    Updated: 2024/08/07 15:43:12 by rdomange         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := bsq

SRC_DIR := ./src
OBJ_DIR := ./obj
HDR := $(SRC_DIR)/bsq.h

CC := cc
CFLAGS := -Wall -Wextra -Werror -I $(SRC_DIR)
RM := rm -rf

SRC := bsq.c norminette.c read.c print.c

SRC := $(patsubst %, $(SRC_DIR)/%, $(SRC))
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HDR) | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

debug: CFLAGS += -g -fsanitize=address,undefined
debug: re

spaced: CFLAGS += -D SPACED=1
spaced: re

.PHONY: all clean fclean re debug spaced
