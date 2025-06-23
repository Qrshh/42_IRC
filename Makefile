CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinc

NAME = ircserv

SRC_DIR = srcs
INC_DIR = inc

SRC =  	main.cpp \
		Server.cpp \
		Channel.cpp \
		Client.cpp


SRCS = $(addprefix $(SRC_DIR)/, $(SRC))

OBJS = $(SRCS:.cpp=.o)

RM = rm -f

all : ${NAME}

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re