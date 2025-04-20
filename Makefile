CFLAGS = -std=c++98 -Wall -Wextra -Werror

CFILES = unicore.cpp \
		core/unicore_http_parse.cpp

HEADERS = core/unicore_defines.hpp \
			core/unicore_buf.hpp \
			core/unicore_http_parse.hpp \
			core/unicore_request.hpp \
			core/unicore_status.hpp 


NAME = webserv

OFILES = $(CFILES:.cpp=.o)

core/%.o: core/%.cpp
	c++ $(CFLAGS) -c $< -o $@

%.o: %.cpp
	c++ $(CFLAGS) -c $<

$(NAME): $(OFILES) $(HEADERS) Makefile
	c++ $(OFILES) -o $@

all: $(NAME)

clean:
	rm -f $(OFILES)

fclean: clean
	rm -f $(NAME)

re: fclean all