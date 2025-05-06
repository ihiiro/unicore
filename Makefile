CFLAGS = -std=c++98 -Wall -Wextra -Werror

CFILES = tests/unicore_tests_parser.cpp \
		core/unicore_http_parse.cpp \
		core/unicore_hash_table.cpp \
		core/unicore_config_parse.cpp 

HEADERS = core/unicore_defines.hpp \
			core/unicore_buf.hpp \
			core/unicore_http_parse.hpp \
			core/unicore_request.hpp \
			core/unicore_status.hpp \
			core/unicore_config_parse.hpp

PARSER_TESTS = parser_tests

NAME = webserv

OFILES = $(CFILES:.cpp=.o)

tests/%.o: tests/%.cpp
	c++ $(CFLAGS) -c $< -o $@

core/%.o: core/%.cpp
	c++ $(CFLAGS) -c $< -o $@

%.o: %.cpp
	c++ $(CFLAGS) -c $<

$(PARSER_TESTS): $(OFILES) $(HEADERS) Makefile
	c++ $(OFILES) -o $@

all: $(PARSER_TESTS)

clean:
	rm -f $(OFILES)

fclean: clean
	rm -f $(PARSER_TESTS)

re: fclean all