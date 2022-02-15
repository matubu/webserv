FLAG = -pthread -Wall -Wextra -Werror -std=c++98 -O3 -fsanitize=address
# -g -fsanitize=address
SRCS = main.cpp
OBJS = $(patsubst %.cpp,bin/%.o,$(SRCS))
NAME = webserv

RED	:=	\033[1;31m
GRE	:=	\033[1;32m
GRA	:=	\033[1;37m
BLU	:=	\033[1;34m
EOC	:=	\033[0m

HEADER_FILES 		= *.hpp
FOLDER_HEADER		= src/

HEADERS				= $(addprefix $(FOLDER_HEADER),$(HEADER_FILES))

bin/%.o: src/%.cpp $(HEADERS)
	@printf "$(BLU)● Compiling $< 🔧$(EOC)\n"
	@mkdir -p bin
	@clang++ $(FLAG) -c $< -o $@
all: $(NAME)
$(NAME): $(OBJS)
	@printf "$(GRE)● Compiling $(NAME) ⚙️ $(EOC)\n"
	@clang++ $(FLAG) $(OBJS) -o $(NAME)
	@printf "$(GRE)● done. ✅ $(EOC)\n"
run: all
	./$(NAME)
clean:
	@printf "$(RED)● Removing objects 📁$(EOC)\n"
	@rm -rf bin
fclean: clean
	@printf "$(RED)● Removing binary ⚙️ $(EOC)\n"
	@rm -rf $(NAME)
re: fclean all

siege_install:
	curl -C - -O http://download.joedog.org/siege/siege-latest.tar.gz
	rm -rf siege
	mkdir siege
	tar -xvf siege-latest.tar.gz -C siege --strip-components 1
	rm -rf  siege-latest.tar.gz
	cd siege && ./configure
	cd siege && make

siege_test_static:
	cd siege/src && ./siege --rc=../../.siegerc http://localhost:8080/www
siege_test_autoindex:
	cd siege/src && ./siege --rc=../../.siegerc http://localhost:8080
siege: siege_test_static

.PHONY: all run clean fclean re \
	siege_install siege_test_static siege_test_autoindex siege