CXX := c++
INCLUDE_FLAGS := -I includes/ -I includes/exceptions -I includes/config
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 ${INCLUDE_FLAGS} -MMD -pedantic -g3

NAME = webserv

SRCS =	srcs/main.cpp \
		srcs/config/configFile.cpp \
		srcs/config/location.cpp \
		srcs/config/server.cpp \
		srcs/utils/atoul.cpp \
		srcs/utils/basename.cpp \
		srcs/utils/is.cpp \
		srcs/utils/itoa.cpp \
		srcs/utils/path_join.cpp \
		srcs/utils/split.cpp \
		srcs/utils/ultoh.cpp \
		srcs/utils/replaceChr.cpp \
		srcs/encoding/entities.cpp \
		srcs/encoding/mime.cpp \
		srcs/http/Request.cpp \
		srcs/http/Response.cpp \
		srcs/http/codes.cpp \
		srcs/io/Socket.cpp \
		srcs/logger/logger.cpp \
		srcs/cgi/Cgi.cpp \
		srcs/cgi/MetaVariable.cpp

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

.cpp:.o
	${CXX} $(CXXFLAGS)  -c $< -o $@

$(NAME): $(OBJS) Makefile
	${CXX} $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

all: $(NAME)

leak-test: all
	valgrind --leak-check=full --show-below-main=yes --show-leak-kinds=all ./$(NAME)

scan-build: fclean clean
	scan-build-12 make | grep "^scan-build:"

pre-push: scan-build leak-test
	make fclean

.PHONY: all clean fclean re scan-build pre-push
