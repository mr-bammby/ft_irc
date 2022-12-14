# written by chelmerd
CXX = c++
CXXFLAGS = -g -Werror -Wall -Wextra -std=c++98 -pedantic-errors

NAME = bot

CLASSES = Server.hpp	\
	Client.hpp	\
	Channel.hpp \
	Message.hpp \
	Comms.hpp \
	ServerResponse.hpp
 
SRCS = \
	main.cpp \
	Bot.cpp

HEADERS = $(addprefix ../srcs/,$(CLASSES)) Bot.hpp

all: $(NAME)

clean:
	$(RM) -f $(NAME)

fclean: clean
	$(RM) -f $(NAME)

re: fclean all

$(NAME): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -I./srcs -o $@

.PHONY: all clean fclean re
