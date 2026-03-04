# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mring <mring@student.42heilbronn.de>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/23 12:47:48 by nsloniow          #+#    #+#              #
#    Updated: 2026/03/04 18:37:16 by mring            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC 		= 			c++
STD		= 			-std=c++17
FLAGS	= 			-Wall -Wextra -Werror #-g -fsanitize=address
NAME	= 			ircserv

# directories
OBJ_DIR	=			obj
INC_DIR	=			includes

SRC		= 			src/main.cpp \
					src/checker/isDigit.cpp \
					src/checker/isSpecial.cpp \
					src/commands/CmdCap.cpp \
					src/commands/CmdJoin.cpp \
					src/commands/CmdNick.cpp \
					src/commands/CmdPass.cpp \
					src/commands/CmdPrivmsg.cpp \
					src/commands/CmdUser.cpp \
					src/commands/CommandDispatcher.cpp \
					src/commands/handleClientInput.cpp \
					src/network/client_user/ClientUser.cpp \
					src/network/client_user/InputBuffer.cpp \
					src/network/client_user/OuputBuffer.cpp \
					src/network/message.cpp \
					src/network/channel/Channel.cpp \
					src/network/server/runServer.cpp \
					src/network/server/Server_channels.cpp \
					src/network/server/server.cpp \
					src/parser/Parser.cpp 
			
OBJ	=				$(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

%.o:				%.cpp
					$(CC) -c $< -o $@
	
all:				$(NAME)

$(NAME):			$(OBJ)
		 			$(CC) $(STD) $(FLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o:		%.cpp | $(OBJ_DIR)
					mkdir -p $(dir $@)
					$(CC) -c $< -o $@

$(OBJ_DIR):
					mkdir -p $(OBJ_DIR)

clean:
					rm -rf $(OBJ_DIR)

fclean:				clean
		 			rm -f $(NAME)

re:					fclean all

.PHONY: all clean fclean re
