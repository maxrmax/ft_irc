# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marvin <marvin@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/23 12:47:48 by nsloniow          #+#    #+#              #
#    Updated: 2026/01/05 17:52:01 by marvin           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++17

# directories
SRC_DIR	=	src/
OBJ_DIR	=	obj/
INC_DIR	=	include/

# source files
SRC_FILES = \
	main.cpp \
	network/Server.cpp \
	network/Socket.cpp \
	network/Poller.cpp \
	client/Client.cpp \
	channel/Channel.cpp \
	parser/Parser.cpp \
	commands/Command.cpp \
	commands/CmdPass.cpp \
	commands/CmdNick.cpp \
	commands/CmdUser.cpp \
	commands/CmdJoin.cpp \
	commands/CmdPrivmsg.cpp \
	commands/CmdQuit.cpp \
	replies/Replies.cpp \
	utils/Utils.cpp

OBJ_FILES = $(SRC_FILES:.cpp=.o)

# paths
SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ = $(addprefix $(OBJ_DIR), $(OBJ_FILES))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

clean:
	@if [ -d "$(OBJ_DIR)" ]; then \
	rm -rf $(OBJ_DIR); \
	fi;

fclean: clean
	@if [ -f "$(NAME)" ]; then \
	rm -f $(NAME); \
	fi;

re: fclean all

.PHONY: all clean fclean re
