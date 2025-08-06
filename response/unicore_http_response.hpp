
#pragma once
#include "../core/unicore_request.hpp"
#include "../core/unicore_config_parse.hpp"
#include <map>
#include <ostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <iostream>
#include <dirent.h>
#include "../server/server.hpp"
#include "../cgi/cgi.hpp"

class client_conn;


typedef struct http_response
{
    std::string http_version;
    int status_code;
    std::string reason_phrase;
    std::map<std::string, std::string> headers;
    std::string body;
}   http_response_t;

void   build_http_response( client_conn &client, int req_line);
void   format_http_response(client_conn &client, http_response_t &response);
int   check_files_errors(client_conn &client, http_response_t &response, std::map<int, std::string> &status_codes, std::string error_number);
std::string  check_path_type(const std::string& path);

