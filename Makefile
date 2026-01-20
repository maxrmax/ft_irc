NAME = ircserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++17

# directories
SRC_DIR	=	source/
OBJ_DIR	=	obj/
INC_DIR	=	includes/

CC 		= 	c++
STD		= 	-std=c++17
# FLAGS	= 	-Wall -Wextra -Werror -g -fsanitize=address
FLAGS	= 	-Wall -Wextra -Werror
NAME	= 	ircserv

# SRC		= 	source/main.cpp \
# 			source/packet/irc_packet.cpp \
# 			source/parsing_check/isDigit.cpp \
# 			source/server/server.cpp \

SRC		= 	src/main.cpp \
			src/checker/isDigit.cpp \
			src/packet/irc_packet.cpp \
			src/server/runServer.cpp \
			src/server/server.cpp \
			
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