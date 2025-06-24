
#pragma once
#include "../core/unicore_request.hpp"
#include "../core/unicore_config_parse.hpp"
#include <map>
#include <ostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <iostream>
#include "../server/server.hpp"
typedef struct http_response
{
    std::string http_version;
    int status_code;
    std::string reason_phrase;
    std::map<std::string, std::string> headers;
    std::string body;
}   http_response_t;

void   build_http_response(unicore_request_t &r, int req_line, int field_line, unicore_config_t &config, client_t &client);
void    format_http_response(client_t &client);

