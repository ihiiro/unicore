CFLAGS = -std=c++98 #-Wall -Wextra -Werror

MAIN_CFILES = main.cpp server/server.cpp server/connection.cpp \
		core/unicore_http_parse.cpp \
		core/unicore_hash_table.cpp \
		core/unicore_config_parse.cpp \
		response/unicore_http_response.cpp 

TESTS_CFILES = tests/unicore_tests_parser.cpp \
		core/unicore_http_parse.cpp \
		core/unicore_hash_table.cpp \
		core/unicore_config_parse.cpp

RESPONSE_CFILES = core/unicore_http_parse.cpp \
		core/unicore_hash_table.cpp \
		core/unicore_config_parse.cpp \
		tests/unicore_tests_response.cpp \


HEADERS = core/unicore_defines.hpp server/server.hpp \
			core/unicore_buf.hpp \
			core/unicore_http_parse.hpp \
			core/unicore_request.hpp \
			core/unicore_config_parse.hpp \
			response/unicore_http_response.hpp \
			server/connection.hpp \

PARSER_TESTS = parser_tests

RESPONSE_TESTS = response_tests

NAME = webserv

RESPONSE_OFILES = $(RESPONSE_CFILES:.cpp=.o)

MAIN_OFILES = $(MAIN_CFILES:.cpp=.o)
RESPONSE_OFILES = $(RESPONSE_CFILES:.cpp=.o)
TESTS_OFILES = $(TESTS_CFILES:.cpp=.o)

response/%.o: response/%.cpp
	c++ -g $(CFLAGS) -c $< -o $@

tests/%.o: tests/%.cpp
	c++ -g $(CFLAGS) -c $< -o $@

core/%.o: core/%.cpp
	c++ -g $(CFLAGS) -c $< -o $@

server/%.o: server/%.cpp
	c++  -g $(CFLAGS) -c $< -o $@

%.o: %.cpp
	c++ -g $(CFLAGS) -c $<

all: $(NAME)

$(RESPONSE_TESTS): $(RESPONSE_OFILES) $(HEADERS) Makefile
	c++ $(RESPONSE_OFILES) -o $@

$(PARSER_TESTS): $(TESTS_OFILES) $(HEADERS) Makefile
	c++ -g $(CFLAGS) $(TESTS_OFILES) -o $@

$(NAME): $(MAIN_OFILES) $(HEADERS) Makefile
	c++ -g $(MAIN_OFILES) ${CFLAGS} -o $@



clean:
	rm -f $(MAIN_OFILES)
	rm -f $(TESTS_OFILES)
	rm -f $(RESPONSE_OFILES)

fclean: clean
	rm -f $(PARSER_TESTS)
	rm -f $(RESPONSE_TESTS)
	rm -f $(NAME)

re: fclean all