CXX := c++
INCLUDE_FLAGS := -I includes/ -I includes/exceptions -I includes/config
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 ${INCLUDE_FLAGS} -MMD -pedantic -fsanitize=address -g3

NAME = config_test

SRCS = main.cpp \
		srcs/utils/atoul.cpp \
		srcs/utils/is.cpp \
		srcs/utils/itoa.cpp \
		srcs/utils/path_join.cpp \
		srcs/utils/split.cpp \
		srcs/utils/ultoh.cpp \
		srcs/utils/basename.cpp \
		srcs/utils/replaceChr.cpp \
		srcs/config/location.cpp \
		srcs/config/configFile.cpp \
		srcs/config/server.cpp \
		srcs/encoding/entities.cpp \
		srcs/encoding/mime.cpp \
		srcs/http/Request.cpp \
		srcs/http/Response.cpp \
		srcs/http/codes.cpp \
		srcs/io/Socket.cpp \
		srcs/logger/logger.cpp \
		srcs/cgi/MetaVariable.cpp \
		srcs/cgi/Cgi.cpp

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

.cpp:.o
	${CXX} $(CXXFLAGS) -c $< -o $@

all: scan-build

$(NAME): $(OBJS) Makefile
	${CXX} $(CXXFLAGS) -o $(NAME) $(OBJS)

quick: $(OBJS) Makefile
	${CXX} $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

scan-build: fclean
	scan-build-12 make -j 4 $(NAME) | grep "^scan-build:"

-include $(DEPS)

.PHONY: all clean fclean re scan-build pre-push
