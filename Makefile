# written by chelmerd
CXX = c++
CXXFLAGS = -Werror -Wall -Wextra -std=c++98 -pedantic-errors

NAME = ircserv

CLASSES = \
	Server.cpp	\
	Client.cpp	\
	Channel.cpp \
	Message.cpp \
	Comms.cpp \
	utility.cpp \
	ServerResponse.cpp

SRCS = \
	main.cpp \
	$(addprefix srcs/,$(CLASSES))

HEADERS = $(patsubst %.cpp,%.hpp,$(addprefix srcs/,$(CLASSES)))

TEST = tests
TESTS = $(wildcard $(TEST)/*.cpp)
TESTBINS = $(patsubst $(TEST)/%.cpp,$(TEST)/bin/%,$(TESTS))

all: $(NAME)

clean:
	$(RM) -f $(NAME).dSYM

fclean: clean
	$(RM) -f $(NAME)
	$(RM) -fr $(TEST)/bin

re: fclean all

$(NAME): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -I./srcs -o $@

test: $(TEST)/bin $(TESTBINS)
	for test in $(TESTBINS) ; do ./$$test ; done

$(TEST)/bin/%: $(TEST)/%.cpp $(addprefix srcs/,$(CLASSES)) | $(TEST)/bin
	$(CXX) $(CXXFLAGS) $< $(addprefix srcs/,$(CLASSES)) -I./srcs -o $@

$(TEST)/bin:
	mkdir $@

.PHONY: all clean fclean re test
