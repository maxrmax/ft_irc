# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nsloniow <nsloniow@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/23 12:47:48 by nsloniow          #+#    #+#              #
#    Updated: 2026/01/27 22:00:43 by nsloniow         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



CC 		= 	c++
STD		= 	-std=c++17
# FLAGS	= 	-Wall -Wextra -Werror -g -fsanitize=address
FLAGS	= 	-Wall -Wextra -Werror
NAME	= 	ircserv

# directories
SRC_DIR	=	source/
OBJ_DIR	=	obj/
INC_DIR	=	includes/

# SRC		= 	source/main.cpp \
# 			source/packet/irc_packet.cpp \
# 			source/parsing_check/isDigit.cpp \
# 			source/server/server.cpp \

SRC		= 	src/main.cpp \
			src/checker/isDigit.cpp \
			src/client/Client.cpp \
			src/commands/CmdNick.cpp \
			src/commands/CommandDispatcher.cpp \
			src/packet/irc_packet.cpp \
			src/network/server/runServer.cpp \
			src/network/server/server.cpp \
			
#            src/parser/Parser.cpp \

OBJ		= 	$(SRC:.cpp=.o)

all		:	$(NAME)
$(NAME)	:	$(OBJ)
		 	$(CC) $(STD) $(FLAGS) $(OBJ) -o $(NAME)

clean	:	
			rm -f $(OBJ)
fclean	:	clean
		 	rm -f $(NAME)
re		:	fclean all

.PHONY: all clean fclean re