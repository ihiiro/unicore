#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <map>
#include "../core/unicore_request.hpp"
#include "../core/unicore_http_parse.hpp"

int execute_cgi(unicore_request_t &req, std::string &result);
