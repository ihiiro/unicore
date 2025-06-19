CFLAGS = -std=c++98 #-Wall -Wextra -Werror

CFILES = main.cpp server/server.cpp \
		core/unicore_http_parse.cpp \
		core/unicore_hash_table.cpp \
		core/unicore_config_parse.cpp

RESPONSE_CFILES = core/unicore_http_parse.cpp \
		core/unicore_hash_table.cpp \
		core/unicore_config_parse.cpp \
		tests/unicore_tests_response.cpp

HEADERS = core/unicore_defines.hpp server/server.hpp \
			core/unicore_buf.hpp \
			core/unicore_http_parse.hpp \
			core/unicore_request.hpp \
			core/unicore_config_parse.hpp

PARSER_TESTS = parser_tests

RESPONSE_TESTS = response_tests

NAME = webserv

RESPONSE_OFILES = $(RESPONSE_CFILES:.cpp=.o)



OFILES = $(CFILES:.cpp=.o)

tests/%.o: tests/%.cpp
	c++ -g $(CFLAGS) -c $< -o $@

core/%.o: core/%.cpp
	c++ -g $(CFLAGS) -c $< -o $@

server/%.o: server/%.cpp
	c++ -g $(CFLAGS) -c $< -o $@

%.o: %.cpp
	c++ -g $(CFLAGS) -c $<

# $(RESPONSE_TESTS): $(RESPONSE_OFILES) $(HEADERS) Makefile
# 	c++ $(RESPONSE_OFILES) -o $@

$(PARSER_TESTS): $(OFILES) $(HEADERS) Makefile
	c++ $(OFILES) -o $@

$(NAME): $(OFILES) $(HEADERS) Makefile
	c++ $(OFILES) ${CFLAGS} -o $@

all: $(NAME)

# all: $(RESPONSE_TESTS)

clean:
	rm -f $(OFILES)

fclean: clean
	rm -f $(PARSER_TESTS)
	rm -f $(RESPONSE_TESTS)
	rm -f $(NAME)

re: fclean all