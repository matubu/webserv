# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/11 02:44:02 by bledda            #+#    #+#              #
#    Updated: 2022/02/11 11:04:06 by bledda           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME				= webserv

#	FILES			############################################################
HEADER_FILES 		= color.h \
						config.hpp \
						utils.hpp

FOLDER_HEADER		= includes/
FOLDER_SRC			= src/
FOLDER_UTILS		= $(addprefix ${FOLDER_SRC},utils/)

SRCS_FILES			=	config.cpp \
							main.cpp

SRCS_UTILS_FILES	=	itos.cpp \
							print.cpp \
							split.cpp \
							strisdigit.cpp \
							strtolower.cpp

SRCS				= $(addprefix ${FOLDER_SRC},${SRCS_FILES})
SRCS_UTILS			= $(addprefix ${FOLDER_UTILS},${SRCS_UTILS_FILES})

SRCS_OBJS			= ${SRCS:.cpp=.o}
SRCS_UTILS_OBJS		= ${SRCS_UTILS:.cpp=.o}

OBJS 				= ${SRCS_OBJS} ${SRCS_UTILS_OBJS}
HEADERS				= $(addprefix ${FOLDER_HEADER},${HEADER_FILES})
################################################################################

#	COMPILATION		############################################################
CC					= clang++
CFLAGS  			= -Wall -Wextra -Werror -std=c++98
RM					= rm -rf
REMAKE				= @make -j --no-print-directory
################################################################################

#	RULES	####################################################################
all:
			$(REMAKE) $(NAME)

$(NAME):	${OBJS}
			@printf $(magenta)
			@printf "\nStart build ${NAME}⏳\n"
			@${CC} ${CFLAGS} ${OBJS} -o ${NAME}
			@printf "${NAME} is ready ✅\n"
			@printf $(reset)

%.o: %.cpp	$(HEADERS)
			@printf $(yellow)
			@printf "Generating ${NAME} objects... %-28.28s\r" $@
			@$(CC) -c $(CFLAGS) -o $@ $<
			@printf $(reset)

re: 		fclean all

clean:
			@${RM} ${OBJS}
			@printf $(magenta)
			@printf "Object files have been deleted 🚮\n"
			@printf $(reset)

fclean:		clean
			@${RM} $(NAME)
			@printf $(magenta)
			@printf "Your folder is now clean 🧹\n"
			@printf $(reset)

.PHONY: 	all clean fclean re
################################################################################

#	COLOR SETTING	############################################################
black 				=	"[1;30m"
red 				=	"[1;31m"
green 				=	"[1;32m"
yellow 				=	"[1;33m"
blue 				=	"[1;34m"
magenta 			=	"[1;35m"
cyan 				=	"[1;36m"
white 				=	"[1;37m"

bg_black 			=	"[40m"
bg_red 				=	"[41m"
bg_green 			=	"[42m"
bg_yellow 			=	"[43m"
bg_blue 			=	"[44m"
bg_magenta 			=	"[45m"
bg_cyan 			=	"[46m"
bg_white 			=	"[47m"

reset 				=	"[0m"
################################################################################
