
CXX = c++
CXXFLAGS = -g -Werror -Wall -Wextra -std=c++98 -pedantic-errors

NAME = ircserv

CLASSES = \
	Server.cpp	\
	Client.cpp	\
	Channel.cpp \
	Message.cpp \
	Comms.cpp \
	ServerResponse.cpp
 
SRCS = \
	main.cpp \
	$(addprefix srcs/,$(CLASSES))

HEADERS = $(patsubst %.cpp,%.hpp,$(addprefix srcs/,$(CLASSES)))

all: $(NAME)

clean:
	$(RM) -f $(NAME)

fclean: clean
	$(RM) -f $(NAME)

re: fclean all

$(NAME): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -I./srcs -o $@

# for bonus 
bot: 
	cd Bot && $(MAKE)

botfclean:
	cd Bot && $(MAKE) fclean

botre:
	cd Bot && $(MAKE) re

.PHONY: all clean fclean re test bot botfclean botre
