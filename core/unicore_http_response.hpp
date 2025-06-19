
#pragma once
#include "unicore_request.hpp"
#include "unicore_config_parse.hpp"
#include <map>
#include <ostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <iostream>
typedef struct http_response
{
    std::string http_version;
    int status_code;
    std::string reason_phrase;
    std::map<std::string, std::string> headers;
    std::string body;
}   http_response_t;

http_response_t    build_http_response(unicore_request_t &r, int req_line, int field_line, unicore_config_t &config);
std::string format_http_response(http_response_t &response);

