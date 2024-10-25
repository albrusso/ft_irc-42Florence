# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: albrusso <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/25 11:58:25 by albrusso          #+#    #+#              #
#    Updated: 2024/09/26 14:34:37 by albrusso         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

################################################################################
#                                  CONFIG                                      #
################################################################################

NAME	=	ircserv
CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror -std=c++98 -g
OBJ_DIR	=	.obj
SIZE	=	30

################################################################################
#                                  COLORS                                      #
################################################################################

RED		=	\033[1;31m
YELLOW	=	\033[1;33m
BLUE	=	\033[1;34m
GREEN	=	\033[1;32m
WHITE	=	\033[1;37m
DEFAULT	=	\033[0m

################################################################################
#                                   FILES                                      #
################################################################################

SRC		=	main.cpp \
			source/Client.cpp \
			source/Channel.cpp \
			source/invite.cpp \
			source/join.cpp \
			source/kick.cpp \
			source/mode.cpp \
			source/privmsg.cpp \
			source/Server.cpp \
			source/topic.cpp \
			source/quit.cpp

OBJ		=	$(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

################################################################################
#                                   MAKE                                       #
################################################################################

all:	$(NAME)
$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@printf "$(WHITE)%s$(BLUE)%-$(SIZE)s$(GREEN)%s$(DEFAULT)\n" "Compiling... " "$<" "[OK]"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/source

clean:
	@for file in $(OBJ); do \
		printf "$(WHITE)%s$(YELLOW)%-$(SIZE)s$(GREEN)%s$(DEFAULT)\n" "Removing...  " "$$file" "[OK]"; \
	done
	@rm -f $(OBJ)
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf "$(WHITE)%s$(RED)%-$(SIZE)s$(GREEN)%s$(DEFAULT)\n" "Removing...  " "$(NAME)" "[OK]"
	@rm -f $(NAME)

re: fclean all

run: $(NAME)
	./$(NAME)

mem: $(NAME)
		valgrind --leak-check=full ./$(NAME)

.PHONY: all clean fclean re run mem