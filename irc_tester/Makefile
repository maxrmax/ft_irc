# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/07 07:25:25 by dfurneau          #+#    #+#              #
#    Updated: 2023/04/15 00:52:29 by dfurneau         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	client-tester

SRC			=	./src/main.cpp \
				./src/parse.cpp \
				./src/client-tester.cpp \


OBJ			=	${SRC:.cpp=.o}
CXX			=	g++

CXXFLAGS	=	-Wall -Wextra -Werror -std=c++2a -Ofast

LIBS		=	-pthread

all:		${NAME}

${NAME}:	${OBJ}

			${CXX} ${CXXFLAGS} ${LIBS} ${OBJ} -o ${NAME}

clean:		
			rm -f ${NAME}

fclean:		clean
			rm -f ${OBJ}

re:			fclean all

tester:
			./client-tester 100 10.13.7.8 6668

tester2:
			./client-tester 100 10.13.7.8 6668 t
.PHONY : all clean re fclean tester tester2
