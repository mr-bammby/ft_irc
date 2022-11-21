# written by chelmerd
CXX = c++
CXXFLAGS = -Werror -Wall -Wextra -std=c++98 -pedantic-errors -Wshadow

NAME = ircserv

CLASSES = \
	srcs/Server.cpp	\
	srcs/Client.cpp	\
	srcs/Channel.cpp

SRCS = \
	main.cpp \
	$(CLASSES)

HEADERS = $(patsubst %.cpp,%.hpp,$(CLASSES))

all: $(NAME)

clean:
	$(RM) -f $(NAME).dSYM

fclean: clean
	$(RM) -f $(NAME)

re: fclean all

$(NAME): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -I./srcs -o $@

.PHONY: all clean fclean re