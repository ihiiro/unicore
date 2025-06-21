#include "../response/unicore_http_response.hpp"

int check_path_type(const std::string& path) {
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0) {
        return -1;
    } else if (S_ISREG(path_stat.st_mode)) {
        //file
       return 1;
    } else if (S_ISDIR(path_stat.st_mode)) {
        //directory
        return 2;
    } else {
        return 0;
    }
}
std::map<int, std::string>& get_all_errors() {
    static std::map<int, std::string> status_codes;
    if (status_codes.empty()) {
        status_codes[100] = "Continue";
        status_codes[101] = "Switching Protocols";
        status_codes[200] = "OK";
        status_codes[201] = "Created";
        status_codes[202] = "Accepted";
        status_codes[203] = "Non-Authoritative Information";
        status_codes[204] = "No Content";
        status_codes[205] = "Reset Content";
        status_codes[206] = "Partial Content";
        status_codes[300] = "Multiple Choices";
        status_codes[301] = "Moved Permanently";
        status_codes[302] = "Found";
        status_codes[303] = "See Other";
        status_codes[304] = "Not Modified";
        status_codes[305] = "Use Proxy";
        status_codes[307] = "Temporary Redirect";
        status_codes[400] = "Bad Request";
        status_codes[401] = "Unauthorized";
        status_codes[402] = "Payment Required";
        status_codes[403] = "Forbidden";
        status_codes[404] = "Not Found";
        status_codes[405] = "Method Not Allowed";
        status_codes[406] = "Not Acceptable";
        status_codes[407] = "Proxy Authentication Required";
        status_codes[408] = "Request Timeout";
        status_codes[409] = "Conflict";
        status_codes[410] = "Gone";
        status_codes[411] = "Length Required";
        status_codes[412] = "Precondition Failed";
        status_codes[413] = "Payload Too Large";
        status_codes[414] = "URI Too Long";
        status_codes[415] = "Unsupported Media Type";
        status_codes[416] = "Range Not Satisfiable";
        status_codes[417] = "Expectation Failed";
        status_codes[426] = "Upgrade Required";
        status_codes[500] = "Internal Server Error";
        status_codes[501] = "Not Implemented";
        status_codes[502] = "Bad Gateway";
        status_codes[503] = "Service Unavailable";
    }
    return status_codes;
}


static std::map<std::string, std::string>& mime_types() {
    static std::map<std::string, std::string> types;
    types[".html"] = "text/html";
    types[".htm"] = "text/html";
    types[".css"] = "text/css";
    types[".js"] = "application/javascript";
    types[".json"] = "application/json";
    types[".png"] = "image/png";
    types[".jpg"] = "image/jpeg";
    types[".jpeg"] = "image/jpeg";
    types[".gif"] = "image/gif";
    types[".svg"] = "image/svg+xml";
    types[".ico"] = "image/x-icon";
    types[".txt"] = "text/plain";
    types[".pdf"] = "application/pdf";
    types[".zip"] = "application/zip";
    return types;
}


//unicore_config_t unicore_request_t soket

http_response_t    build_http_response(unicore_request_t &r, int req_line, int field_line, unicore_config_t &config)
{
    http_response_t response;
    std::string extension = (char *)r.static_uri_path;
    size_t pos = extension.find_last_of('.');
    if (pos != std::string::npos) {
        extension = extension.substr(pos);
    }
    bucket *bucket;
    std::map<int, std::string> &status_codes = get_all_errors();
    std::map<std::string, std::string> &mime_types_map = mime_types();
    response.http_version = "HTTP/1.1";
    //checking for route existence int config later
    std::string a = std::string ( r.route->root );
    // std::cout << "root path: " << a << "\n";
    std::string b = (char *)r.static_uri_path;
    std::string uri_req = "." + a + b;
    std::cout << "uri_req: " << uri_req << "\n";
    std::ifstream file(uri_req.c_str());

    if (!file.is_open() && req_line == -1)
    {
        response.status_code = 404; // Not Found
        response.reason_phrase = status_codes[404];
        response.body = "File not found: " + uri_req + "\n";
        response.headers["Content-Type"] = "text/plain";
        response.headers["Content-Length"] = std::to_string(response.body.size());
        response.headers["Connection"] = "close";
    }
    else
    {
        std::cout << "response entred \n";
        bucket = get(config.redirection_list, r.static_uri_path);
        int path_type = check_path_type(uri_req);
        //redirect or not
        if (bucket && bucket->value != NULL)
        {
            std::cout << "Redirecting to: " << (char *)bucket->value << "\n";
            response.status_code = 301; // Moved Permanently
            response.reason_phrase = status_codes[301];
            response.headers["Location"] = (char *)bucket->value;
            response.body = "Redirecting to: " + std::string((char *)bucket->value) + "\n";
            response.headers["Content-Type"] = "text/plain";
            response.headers["Content-Length"] = std::to_string(response.body.size());
            response.headers["Connection"] = "close";
        }
        else
        {
            if (r.route->ROUTE_GET || r.route->ROUTE_POST || r.route->ROUTE_DELETE)
            {
                if (r.REQUEST_METHOD == GET && r.route->ROUTE_GET)
                {
                    if (path_type == 2)
                    {
                        // If it's a directory, check for index.html or similar
                        file.close();
                        uri_req += "/index.html"; // or whatever default file you want to serve
                        file.open(uri_req.c_str());
                        if (!file.is_open())
                        {
                            response.status_code = 404; // Not Found
                            response.reason_phrase = status_codes[404];
                            response.body = "Directory index not found: " + uri_req + "\n";
                            response.headers["Content-Type"] = "text/plain";
                            response.headers["Content-Length"] = std::to_string(response.body.size());
                            response.headers["Connection"] = "close";
                        }
                        else
                        {
                            std::ostringstream ss;
                            ss << file.rdbuf();
                            response.body = ss.str();
                            response.status_code = 200;
                            response.reason_phrase = status_codes[200];
                            response.headers["Content-Type"] = mime_types_map[extension]; // or detect via file extension
                            response.headers["Content-Length"] = std::to_string(response.body.size());
                            response.headers["Connection"] = "close";
                        }
                    }
                    else if (path_type == 1)
                    {
                        std::ostringstream ss;
                        ss << file.rdbuf();
                        response.body = ss.str();
                        response.status_code = 200;
                        response.reason_phrase = status_codes[200];
                        response.headers["Content-Type"] = mime_types_map[extension]; // or detect via file extension
                        response.headers["Content-Length"] = std::to_string(response.body.size());
                        response.headers["Connection"] = "close";
                    }
                }
                // else if (r.REQUEST_METHOD == POST && r.route->ROUTE_POST)
                // {
                //     // Handle POST request
                // }
                // else if (r.REQUEST_METHOD == POST && !r.route->ROUTE_POST )
                // {
                //     // Handle POST request but route does not support it
                // }
                // else if (r.REQUEST_METHOD == DELETE && r.route->ROUTE_DELETE)
                // {
                //     // Handle DELETE request
                // }
                else
                {
                    response.status_code = 405; // Method Not Allowed
                    response.reason_phrase = status_codes[405];
                    response.body = "Method not allowed for this route.\n";
                    response.headers["Content-Type"] = "text/plain";
                    response.headers["Content-Length"] = std::to_string(response.body.size());
                    response.headers["Connection"] = "close";
                }
            }
        }
        //print the response

    }
    return response;
}

std::string format_http_response(http_response_t &response) {
    std::ostringstream oss;
    oss << response.http_version << " " << response.status_code << " " << response.reason_phrase << "\r\n";
//c++98
    for (std::map<std::string, std::string>::const_iterator it = response.headers.begin(); it != response.headers.end(); ++it) {
        oss << it->first << ": " << it->second << "\r\n";
    }
    oss << "\r\n"; // End of headers
    oss << response.body; // Body content
    return oss.str();
}