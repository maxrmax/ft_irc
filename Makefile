NAME = ircserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++17

# directories
SRC_DIR	=	source/
OBJ_DIR	=	obj/
INC_DIR	=	includes/

# source files
SRC_FILES = \
	main.cpp \
	parser/Parser.cpp \
	

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
