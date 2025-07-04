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
        //file
       return "file";
    } else if (S_ISDIR(path_stat.st_mode)) {
        //directory
        return "directory";
    } else {
        return "Not found";
    }
}
std::map<int, std::string>& get_all_errors() {
    static std::map<int, std::string> status_codes;
        status_codes[100] = "<html><body><h1>Continue</h1><body></html>";
        status_codes[101] = "<html><body><h1>Switching Protocols</h1><body></html>";
        status_codes[200] = "<html><body><h1>OK</h1><body></html>";
        status_codes[201] = "<html><body><h1>Created</h1><body></html>";
        status_codes[202] = "<html><body><h1>Accepted</h1><body></html>";
        status_codes[203] = "<html><body><h1>Non-Authoritative Information</h1><body></html>";
        status_codes[204] = "<html><body><h1>No Content</h1><body></html>";
        status_codes[205] = "<html><body><h1>Reset Content</h1><body></html>";
        status_codes[206] = "<html><body><h1>Partial Content</h1><body></html>";
        status_codes[300] = "<html><body><h1>Multiple Choices</h1><body></html>";
        status_codes[301] = "<html><body><h1>Moved Permanently</h1><body></html>";
        status_codes[302] = "<html><body><h1>Found</h1><body></html>";
        status_codes[303] = "<html><body><h1>See Other</h1><body></html>";
        status_codes[304] = "<html><body><h1>Not Modified</h1><body></html>";
        status_codes[305] = "<html><body><h1>Use Proxy</h1><body></html>";
        status_codes[307] = "<html><body><h1>Temporary Redirect</h1><body></html>";
        status_codes[400] = "<html><body><h1>Bad Request</h1><body></html>";
        status_codes[401] = "<html><body><h1>Unauthorized</h1><body></html>";
        status_codes[402] = "<html><body><h1>Payment Required</h1><body></html>";
        status_codes[403] = "<html><body><h1>Forbidden</h1><body></html>";
        status_codes[404] = "<html><body><h1>Not Found</h1><body></html>";
        status_codes[405] = "<html><body><h1>Method Not Allowed</h1><body></html>";
        status_codes[406] = "<html><body><h1>Not Acceptable</h1><body></html>";
        status_codes[407] = "<html><body><h1>Proxy Authentication Required</h1><body></html>";
        status_codes[408] = "<html><body><h1>Request Timeout</h1><body></html>";
        status_codes[409] = "<html><body><h1>Conflict</h1><body></html>";
        status_codes[410] = "<html><body><h1>Gone</h1><body></html>";
        status_codes[411] = "<html><body><h1>Length Required</h1><body></html>";
        status_codes[412] = "<html><body><h1>Precondition Failed</h1><body></html>";
        status_codes[413] = "<html><body><h1>Payload Too Large</h1><body></html>";
        status_codes[414] = "<html><body><h1>URI Too Long</h1><body></html>";
        status_codes[415] = "<html><body><h1>Unsupported Media Type</h1><body></html>";
        status_codes[416] = "<html><body><h1>Range Not Satisfiable</h1><body></html>";
        status_codes[417] = "<html><body><h1>Expectation Failed</h1><body></html>";
        status_codes[426] = "<html><body><h1>Upgrade Required</h1><body></html>";
        status_codes[500] = "<html><body><h1>Internal Server Error</h1><body></html>";
        status_codes[501] = "<html><body><h1>Not Implemented</h1><body></html>";
        status_codes[502] = "<html><body><h1>Bad Gateway</h1><body></html>";
        status_codes[503] = "<html><><h1>Service Unavailable</h1><body></html>";
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

void    getmethod(client_conn &client, http_response_t &response, std::map<int, std::string> &status_codes, std::map<std::string, std::string> &mime_types_map)
{
    bucket *bucket;
    std::string path, root, static_uri_path;
    root = client.request.route->root;
    static_uri_path = (char *)client.request.static_uri_path;
    path = "." + root + static_uri_path;
    //check if file exists
    std::string type = check_path_type(path);
    if(type == "file")
    {
        client.filename = path;
        response.status_code = 200;
        response.reason_phrase = "OK";
        std::string ext = path.substr(path.find_last_of('.'));
        if (mime_types_map.find(ext) != mime_types_map.end())
            response.headers["Content-Type"] = mime_types_map[ext];
        else
            response.headers["Content-Type"] = "application/octet-stream";
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            const u_char* error = (u_char *)"404";
            // bucket = get(client.info.error_pages, error);
            if (bucket)
                client.filename = (char *)bucket->value;
            response.status_code = 404;
            response.reason_phrase = "Not Found";
            response.headers["Content-Type"] = "text/html";
        }
    }
    else if (type == "directory")
    {
        if (client.request.route->directory_listing)
        {
            response.status_code = 200;
            response.reason_phrase = "OK";
            response.headers["Content-Type"] = "text/html";
        }
        else
        {
            const u_char *error = (u_char *)"403";
            // bucket = get(client.info.error_pages, error);
            if (bucket)
                client.filename = (char *)bucket->value;
            response.status_code = 403;
            response.reason_phrase = "Forbidden";
            response.headers["Content-Type"] = "text/html";
        }
    }
    else
    {
        const u_char *error = (u_char *)"404";
        bucket = get(client.info.error_pages, error);
        if (bucket)
            client.filename = (char *)bucket->value;
        response.status_code = 404;
        response.reason_phrase = "Not Found";
        response.headers["Content-Type"] = "text/html";
    }
}

void     build_http_response(client_conn &client, int req_line)
{

    //print client.request
    // std::cout << "Building HTTP response for request line: " << req_line << std::endl;
    // std::cout << "Request Method: " << client.request.REQUEST_METHOD << std::endl;
    // std::cout << "Request URI: " << client.request.static_uri_path << std::endl;
    http_response_t response;
    if (!client.chunked)
    {
        std::map<std::string, std::string> &mime_types_map = mime_types();
        std::map<int, std::string> &status_codes = get_all_errors();
        bucket          *bucket;
        // std::string path;
        std::string root;
        std::string static_uri_path;









        response.http_version = "HTTP/1.1";
        //set the path
        root = client.request.route->root;
        // static_uri_path = (char *)client.request.static_uri_path;
        //check if redirections
        bucket = get(client.info.redirection_list, client.request.static_uri_path);
        if (bucket && bucket->value)
        {
            static_uri_path = (char *)bucket->value;
            response.status_code = 301;
            response.reason_phrase = "Moved Permanently";
            response.headers["Location"] = static_uri_path;
            response.headers["Content-Type"] = "text/html";
            response.body = status_codes[response.status_code];
        }
        else
        {
            int method = client.request.REQUEST_METHOD;
            if (method == GET || method == POST || method == DELETE)
            {
                if (method == GET && client.request.route->ROUTE_GET)
                    getmethod(client, response, status_codes, mime_types_map);
                // else if (method == POST && client.request.route->ROUTE_POST)
                //     postmethod(client, response);
                // else if (method == DELETE && client.request.route->ROUTE_DELETE)
                //     deletemethod(client, response);
                else
                {
                    response.status_code = 405;
                    response.reason_phrase = "Method Not Allowed";
                    response.headers["Content-Type"] = "text/html";
                    response.body = status_codes[response.status_code];
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
    const std::size_t CHUNK_SIZE = 1024;

    // First-time response (headers)
    std::cerr << "client.chunked: " << client.chunked << std::endl;
    if (!client.chunked)
    {
        std::cout << "not chunked" << std::endl;
        std::ifstream file(client.filename.c_str(), std::ios::binary);
        oss << response.http_version << " " << response.status_code << " " << response.reason_phrase << "\r\n";
        for (std::map<std::string, std::string>::iterator it = response.headers.begin(); it != response.headers.end(); ++it) {
            oss << it->first << ": " << it->second << "\r\n";
        }

        std::streamsize file_size = get_file_size(client.filename);
        if (file_size >= 0 && file_size < CHUNK_SIZE)
        {
            std::cerr << "File size is less than CHUNK_SIZE, sending entire file in one response." << std::endl;
            oss << "Content-Length: " << client.filename.size() << "\r\n\r\n";
            std::ifstream file(client.filename.c_str(), std::ios::binary);
            oss << file.rdbuf();
            file.close();
            oss << "\r\n"; // End of body
            client.offset = -1337;
        }
        else
        {
            oss << "Transfer-Encoding: chunked\r\n\r\n";
            client.chunked = true; // Set flag for next call
        }
    }

    // Handle chunked body transfer
    if (client.chunked)
    {
        std::cout << "chunked transfer" << std::endl;
        std::ifstream file(client.filename.c_str(), std::ios::binary);
        if (file.is_open())
        {
            file.seekg(client.offset, std::ios::beg);

            char buffer[CHUNK_SIZE];
            std::streamsize bytes_read = file.rdbuf()->sgetn(buffer, CHUNK_SIZE);

            if (bytes_read > 0)
            {
                oss << std::hex << bytes_read << "\r\n";
                oss.write(buffer, bytes_read);
                oss << "\r\n";
            }

            // End of file
            if (file.eof() || bytes_read == 0)
            {
                std::cerr << "End of file reached, sending final chunk." << std::endl;
                oss << "0\r\n\r\n"; // Terminate chunked stream
                client.chunked = false; // Optional: reset chunked flag
                client.offset = -1337; // Reset offset for next request
            }
        }
    }

    client.getBuffer() = oss.str();
}