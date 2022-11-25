# written by chelmerd
CXX = c++
CXXFLAGS = -Werror -Wall -Wextra -std=c++98 -pedantic-errors -Wshadow

NAME = ircserv

CLASSES = \
<<<<<<< HEAD
	srcs/Server.cpp	\
	srcs/Client.cpp	\
	srcs/Channel.cpp \
	srcs/Comms.cpp \
=======
	Server.cpp	\
	Client.cpp	\
	Channel.cpp \
	Message.cpp
>>>>>>> 8016ce84b354d59b393fc469f7dbb7d9abab7cd1

SRCS = \
	main.cpp \
	$(addprefix srcs/,$(CLASSES))

HEADERS = $(patsubst %.cpp,%.hpp,$(addprefix srcs/,$(CLASSES)))

all: $(NAME)

clean:
	$(RM) -f $(NAME).dSYM

fclean: clean
	$(RM) -f $(NAME)

re: fclean all

$(NAME): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -I./srcs -o $@

.PHONY: all clean fclean re
