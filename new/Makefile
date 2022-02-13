FLAG = -pthread -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address
SRCS = main.cpp
OBJS = $(patsubst %.cpp,bin/%.o,$(SRCS))
NAME = webserv

RED	:=	\033[1;31m
GRE	:=	\033[1;32m
GRA	:=	\033[1;37m
BLU	:=	\033[1;34m
EOC	:=	\033[0m

bin/%.o: src/%.cpp
	@echo "$(BLU)● Compiling $^ 🔧$(EOC)"
	@mkdir -p bin
	@clang++ $(FLAG) -c $^ -o $@
all: $(NAME)
$(NAME): $(OBJS)
	@echo "$(GRE)● Compiling $(NAME) ⚙️ $(EOC)"
	@clang++ $(FLAG) $(OBJS) -o $(NAME)
	@echo "$(GRE)● done. ✅ $(EOC)"
run: all
	./$(NAME)
clean:
	@echo "$(RED)● Removing objects 📁$(EOC)"
	@rm -rf bin
fclean: clean
	@echo "$(RED)● Removing binary ⚙️ $(EOC)"
	@rm -rf $(NAME)
re: fclean all

.PHONY: all run clean fclean re