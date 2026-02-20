# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ngoyat <ngoyat@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/23 12:47:48 by nsloniow          #+#    #+#              #
#    Updated: 2026/02/20 16:13:19 by ngoyat           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



CC 		= 	c++
STD		= 	-std=c++17
# FLAGS	= 	-Wall -Wextra -Werror -g -fsanitize=address
FLAGS	= 	-Wall -Wextra -Werror
NAME	= 	ircserv

# directories
OBJ_DIR	=	obj/
INC_DIR	=	includes/

SRC		= 	src/main.cpp \
			src/checker/isDigit.cpp \
			src/checker/isSpecial.cpp \
			src/commands/CmdCap.cpp \
			src/commands/CmdNick.cpp \
			src/commands/CmdUser.cpp \
			src/commands/CommandDispatcher.cpp \
			src/commands/handleClientInput.cpp \
			src/network/client_user/ClientUser.cpp \
			src/network/client_user/InputBuffer.cpp \
			src/network/client_user/OuputBuffer.cpp \
			src/network/message.cpp \
			src/network/server/runServer.cpp \
			src/network/server/server.cpp \
			src/packet/irc_packet.cpp \
            src/parser/Parser.cpp \
			
OBJ		= 	$(SRC:.cpp=.o)

%.o : %.cpp
	$(CC) $(STD) $(FLAGS) -c $< -o $@
	
all		:	$(NAME)
$(NAME)	:	$(OBJ)
		 	$(CC) $(STD) $(FLAGS) $(OBJ) -o $(NAME)

clean	:	
			rm -f $(OBJ)
fclean	:	clean
		 	rm -f $(NAME)
re		:	fclean all

.PHONY: all clean fclean re