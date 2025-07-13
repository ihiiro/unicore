#include "../response/unicore_http_response.hpp"

std::streamsize get_file_size(const std::string& filename)
{
    std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
    if (!file)
        return -1;

    std::streamsize size = file.tellg();
    file.close();
    return size;
}

std::string  check_path_type(const std::string& path) {
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0) {
        return "unaccessible";
    } else if (S_ISREG(path_stat.st_mode)) {
       return "file";
    } else if (S_ISDIR(path_stat.st_mode)) {
        return "directory";
    } else {
        return "Not found";
    }
}
std::map<int, std::string>& get_all_errors() {
    static std::map<int, std::string> status_codes;
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
//--------------------------------------------------------------------ERROR---------FILES--------------------------------------------------------------------//
void    check_files_errors(client_conn &client, http_response_t &response, std::map<int, std::string> &status_codes, std::string error_number)
{
    bucket *buck;

    const u_char *error = (const u_char *)error_number.c_str();
    buck = get(client.info.error_pages, error);
    if (buck && buck->value)
    {
        std::cout << "bucket->value: " << (char *)buck->value << std::endl;
        client.filename  =  "." + std::string((char *)buck->value);
        if (!std::ifstream(client.filename).is_open())
        {
            std::cout << "could not open file: " << client.filename << std::endl;
            response.status_code = 404;
            response.reason_phrase = "Not Found";
            response.headers["Content-Type"] = "text/html";
            response.body = " <html><body><h1>cant be opened </h1></body></html>";
            return;
        }
        else
        {
            response.status_code = std::stoi(error_number);
            response.headers["Content-Type"] = mime_types()[client.filename.substr(client.filename.find_last_of('.'))];
        }
    }
    else
    {
        response.status_code = std::stoi(error_number);
        response.reason_phrase = status_codes[response.status_code];
        response.headers["Content-Type"] = "text/html";
        response.body = "<!DOCTYPE html>\n<html><body><h1>" + status_codes[response.status_code]+ "</h1></body></html>";
    }
}
//--------------------------------------------------------------------GET________________METHOD-----------------------------------------------------------------------------//
void    getmethod(client_conn &client, http_response_t &response, std::map<int, std::string> &status_codes, std::map<std::string, std::string> &mime_types_map)
{
    std::cout << "getmethod called" << std::endl;
    bucket *bucket;
    std::string path, root, static_uri_path;
    root = client.request.route->root;
    static_uri_path = (char *)client.request.static_uri_path;
    path = "." + root + static_uri_path;
    //check if file exists
    std::string type = check_path_type(path);
    std::cout << "type : " << type << std::endl;
    std::cout << "path : " << path << std::endl;
    std::cout << " static_uri_path : " <<client.request.static_uri_path<< std::endl;
    if(type == "file")
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            check_files_errors(client, response, status_codes, "404");
            return;
        }
        client.filename = path;
        response.status_code = 200;
        response.reason_phrase = "OK";
        std::string ext = path.substr(path.find_last_of('.'));
        if (mime_types_map.find(ext) != mime_types_map.end())
            response.headers["Content-Type"] = mime_types_map[ext];
        else
            response.headers["Content-Type"] = "application/octet-stream";
    }
    else if (type == "directory")
    {
        std::string static_uri_path = (char *)client.request.static_uri_path;
        if (static_uri_path.back() != '/')
        {
            response.status_code = 301;
            response.reason_phrase = "Moved Permanently";
            response.headers["Location"] = static_uri_path+ "/";
            return;
        }
        if (client.request.route->file_if_directory_request)
        {
            std::cout << "there is a file_if_directory_request" << std::endl;
            client.filename = path + client.request.route->file_if_directory_request;
            std::cout << "client.filename2: " << client.filename << std::endl;
            std::ifstream file(client.filename, std::ios::binary);
            if (!file.is_open())
            {
                std::cout << "could not open file: " << client.filename << std::endl;
                check_files_errors(client, response, status_codes, "404");
                return;
            }
            response.status_code = 200;
            response.reason_phrase = "OK";
            std::string ext = client.filename.substr(client.filename.find_last_of('.'));
            if (mime_types_map.find(ext) != mime_types_map.end())
                response.headers["Content-Type"] = mime_types_map[ext];
            else
                response.headers["Content-Type"] = "application/octet-stream";
            return;
        }
        else if (client.request.route->directory_listing)
        {
            std::cout << "there is a directory listing" << std::endl;
            DIR *dir = opendir(path.c_str());
            if (dir == NULL)
            {
                check_files_errors(client, response, status_codes, "403");
                return;
            }
            std::ostringstream html;
            html << "<html><head><title>Directory Listing</title></head><body>";
            html << "<h1>Directory Listing for " << static_uri_path << "</h1><ul>";

            struct dirent *entry;
            while ((entry = readdir(dir)) != NULL)
            {
                std::string name = entry->d_name;
                if (name == "." || name == "..")
                    continue;
                html << "<li><a href=\"" << static_uri_path << "/" << name << "\">" << name << "</a></li>";
            }

            closedir(dir);

            html << "</ul></body></html>";

            response.status_code = 200;
            response.reason_phrase = "OK";
            response.headers["Content-Type"] = "text/html";
            response.body = html.str();

        }
        else
            check_files_errors(client, response, status_codes, "403");
    }
    else
        check_files_errors(client, response, status_codes, "404");
}
//--------------------------------------------------------------------DELETE________________METHOD-----------------------------------------------------------------------------//
bool remove_directory_recursive(const std::string &path)
{
    DIR *dir = opendir(path.c_str());
    if (!dir)
        return false;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        std::string entry_path = path + "/" + entry->d_name;
        struct stat entry_stat;
        if (stat(entry_path.c_str(), &entry_stat) == 0)
        {
            if (S_ISDIR(entry_stat.st_mode))
            {
                if (!remove_directory_recursive(entry_path))
                {
                    closedir(dir);
                    return false;
                }
            }
            else
            {
                if (remove(entry_path.c_str()) != 0)
                {
                    closedir(dir);
                    return false;
                }
            }
        }
    }
    closedir(dir);
    return rmdir(path.c_str()) == 0;
}
void    deletemethod(client_conn &client, http_response_t &response)
{
    std::cout << "deletemethod called" << std::endl;
    bucket *bucket;
    std::string path, root, static_uri_path;
    root = client.request.route->root;
    static_uri_path = (char *)client.request.static_uri_path;
    path = "." + root + static_uri_path;
    //check if file exists
    std::string type = check_path_type(path);
    std::cout << "type : " << type << std::endl;
    std::cout << "path : " << path << std::endl;
    if(type == "file")
    {
        if (remove(path.c_str()) == 0)
        {
            response.status_code = 204;
            response.reason_phrase = "No Content";
            response.headers["Content-Length"] = "0";
        }
        else
        {
            check_files_errors(client, response, get_all_errors(), "500");
        }
    }
    else if (type == "directory")
    {
        if (path.back() != '/')
        {
            check_files_errors(client, response, get_all_errors(), "409");
            return;
        }
        if (access(path.c_str(), W_OK) == 0)
        {
            if (remove_directory_recursive(path))
            {
                response.status_code = 204;
                response.reason_phrase = "No Content";
                response.headers["Content-Length"] = "0";
            }
            else
                check_files_errors(client, response, get_all_errors(), "500");
        }
        else
        {
            check_files_errors(client, response, get_all_errors(), "403");
        }
    }
    else
    {
        check_files_errors(client, response, get_all_errors(), "404");
    }
}
//--------------------------------------------------------------------POST________________METHOD-----------------------------------------------------------------------------//
void    postmethod(client_conn &client, http_response_t & response, int req_line)
{
    if (!client.request.route->ROUTE_POST)
    {
        std::string path = "." + std::string(client.request.route->root) + std::string((char *)client.request.static_uri_path);
        if (!std::ifstream(path).is_open())
        {
            check_files_errors(client, response, get_all_errors(), "404");
            return;
        }
        else
        {
            std::string type = check_path_type(path);
            if (type == "file")
            {
                check_files_errors(client, response, get_all_errors(), "403");
            }
            else if (type == "directory")
            {
                std::string uri = (char *)client.request.static_uri_path;
                if (uri.back() != '/')
                {
                    response.status_code = 301;
                    response.reason_phrase = "Moved Permanently";
                    response.headers["Location"] = uri + "/";
                    return;
                }
                else
                {
                    if (client.request.route->file_if_directory_request)
                    {
                        std::string file_path = path + client.request.route->file_if_directory_request;
                        if (!std::ifstream(file_path).is_open())
                        {
                            check_files_errors(client, response, get_all_errors(), "403");
                            return;
                        }
                    }
                    else
                    {
                        check_files_errors(client, response, get_all_errors(), "403");
                        return;
                    }
                }
            }
            else
            {
                check_files_errors(client, response, get_all_errors(), "404");
                return;
            }
        }
    }
    else
    {
        std::cout << "postmethod called" << std::endl;
        std::cout << "req_line: " << req_line << std::endl;
        std::ostringstream oss;
        oss << req_line;
        std::string req_line_str = oss.str();
        check_files_errors(client, response, get_all_errors(), req_line_str);
    }
}
//--------------------------------------------------------------------BUILD________________HTTP_RESPONSE-----------------------------------------------------------------------------//
void     build_http_response(client_conn &client, int req_line)
{
    http_response_t response;

    if (req_line >= 100 && req_line < 600 && client.request.REQUEST_METHOD != POST)
    {
        std::ostringstream oss;
        oss << req_line;
        std::string req_line_str = oss.str();
        check_files_errors(client, response, get_all_errors(), req_line_str);
    }

    if (!client.chunked)
    {
        std::map<std::string, std::string> &mime_types_map = mime_types();
        std::map<int, std::string> &status_codes = get_all_errors();
        bucket          *headers_bucket;
        bucket          *bucket;
        std::string root;
        std::string static_uri_path;


        headers_bucket = get(client.request.headers, (const u_char *)"connection");
        if (headers_bucket && headers_bucket->value)
        {
            if (!std::strcmp((char *)headers_bucket->value, "keep-alive"))
            {
                response.headers["Connection"] = "keep-alive";
                client.keep_alive = true;
            }
            else
            {
                client.keep_alive = false;
                response.headers["Connection"] = "close";
            }
        }
        else
        {
            response.headers["Connection"] = "keep-alive";
            client.keep_alive = true;
        }
        response.http_version = "HTTP/1.1";
        root = client.request.route->root;
        bucket = get(client.info.redirection_list, client.request.absolute_path);
        if (bucket && bucket->value)
        {
            static_uri_path = (char *)bucket->value;
            response.status_code = 301;
            response.headers["Connection"] = "keep-alive";
            response.headers["Content-Length"] = "0";
            response.reason_phrase = "Moved Permanently";
            response.headers["Location"] = static_uri_path;
        }
        else
        {
            std::cout << "the redirection list is empty" << std::endl;
            int method = client.request.REQUEST_METHOD;
            std::cout << "the method is: " << method << std::endl;
            if (method == GET || method == POST || method == DELETE)
            {
                if (method == GET && client.request.route->ROUTE_GET)
                    getmethod(client, response, status_codes, mime_types_map);
                else if (method == POST && client.request.route->ROUTE_POST)
                    postmethod(client, response, req_line);
                else if (method == DELETE && client.request.route->ROUTE_DELETE)
                    deletemethod(client, response);
                else
                {
                    check_files_errors(client, response, status_codes, "405");
                }
            }
        }
    }
    format_http_response(client, response);
}

void format_http_response(client_conn &client, http_response_t &response)
{
    std::ostringstream oss;
    std::string &buffer = client.getBuffer();
    buffer.clear();
    const std::size_t CHUNK_SIZE = 65536;
    std::cout << "client.filename: ' " << client.filename << "'"<< std::endl;

    // First-time response (headers)
    std::cerr << "client.chunked: " << client.chunked << std::endl;
    if (!client.chunked)
    {
        std::cout << "not chunked" << std::endl;
        if (client.filename != "")
        {
            std::cout << "client.filename: " << client.filename<<" not empty " << std::endl;
            std::ifstream file(client.filename.c_str(), std::ios::binary);
            if (!file.is_open())
                std::cout << "could not open file: " << client.filename << std::endl;
        }
        oss << response.http_version << " " << response.status_code << " " << response.reason_phrase << "\r\n";
        for (std::map<std::string, std::string>::iterator it = response.headers.begin(); it != response.headers.end(); ++it) {
            oss << it->first << ": " << it->second << "\r\n";
        }
        if (client.filename != "")
        {
            std::streamsize file_size = get_file_size(client.filename);
            std::cout << "file_size: " << file_size << std::endl;
            if (file_size >= 0 && file_size < CHUNK_SIZE)
            {
                std::cerr << "File size is less than CHUNK_SIZE, sending entire file in one response." << std::endl;
                oss << "Content-Length: " << file_size << "\r\n";
                std::ifstream file(client.filename.c_str(), std::ios::binary);
                oss << file.rdbuf();
                file.close();
                client.offset = -1337;
            }
            else
            {
                if (file_size < 0)
                {
                    if (response.body != "")
                    {
                        std::cerr << "File size could not be determined, using body content." << std::endl;
                        oss << "Content-Length: " << response.body.size() << "\r\n\r\n";
                        oss << response.body; // Add body content
                        client.offset = -1337; // Reset offset for next request
                    }
                    else
                    {
                        std::cerr << "File size could not be determined and response body is empty." << std::endl;
                        oss << "Content-Length: 0\r\n\r\n"; // No body content
                    }
                }
                else
                {
                    std::cout << "it is chunked" << std::endl;
                    oss << "Transfer-Encoding: chunked\r\n\r\n";
                    client.chunked = true;
                }
            }
        }
        else
        {
            if (response.body != "")
            {
                std::cout << "response.body is not empty" << std::endl;
                oss << "Content-Length: " << response.body.size() << "\r\n\r\n";
                oss << response.body; // Add body content
            }
            else
            {
                std::cout << "response.body is empty" << std::endl;
            }
            oss << "\r\n"; // End of headers
            client.getBuffer() = oss.str();
            client.offset = -1337; // Reset offset for next request
        }
    }

    if (client.chunked)
    {
        std::cout << "chunked transfer" << std::endl;
        std::ifstream file(client.filename.c_str(), std::ios::binary);
        if (file.is_open())
        {
            std::cout << "file is open" << std::endl;
            file.seekg(client.offset, std::ios::beg);

            char *buffer_chunk = new char[CHUNK_SIZE];
            std::memset(buffer_chunk, 0, CHUNK_SIZE);
            std::streamsize bytes_read = file.rdbuf()->sgetn(buffer_chunk, CHUNK_SIZE);

            if (bytes_read > 0)
            {
                oss << std::hex << bytes_read;
                oss << "\r\n";
                oss.write(buffer_chunk, bytes_read);
                oss << "\r\n";
                client.offset += bytes_read; // Update offset for next read
            }

            // End of file
            if (file.eof() || bytes_read == 0)
            {
                std::cerr << "End of file reached, sending final chunk." << std::endl;
                oss << "0\r\n\r\n"; // Terminate chunked stream
                client.chunked = false; // Optional: reset chunked flag
                client.offset = -1337; // Reset offset for next request
            }
            delete[] buffer_chunk;
        }
        else
            std::cout << "could not open file : "<< client.filename << std::endl;
    }
    client.getBuffer() = oss.str();
}

// int cgi (response_t &response, client_conn &client)