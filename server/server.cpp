#include "server.hpp"

server::server(const server &other)
    : sockfd(other.sockfd), listen_sockfd(other.listen_sockfd), failed(other.failed),
      port(other.port), host(other.host), info(other.info)
{
}

int create_and_bind_socket(int port, const char* ip)
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    int opt = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0 && ip != std::string("localhost"))
    {
        std::cerr << "Invalid IP address: " << ip << std::endl;
        close(socketfd);
        return -1;
    }
    if (ip == std::string("localhost"))
    {
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }
    else
    {
        addr.sin_addr.s_addr = inet_addr(ip);
    }

    int flags = fcntl(socketfd, F_GETFL, 0);
    int result = fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
    if (result < 0)
    {
        std::cerr << "Error setting socket to non-blocking mode" << std::endl;
        close(socketfd);
        return -1;
    }
    if (bind(socketfd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Error binding socket to port " << port << std::endl;
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return -1;
    }
    return socketfd;
}

WebServer::WebServer(const std::string &configPath) :  config_file(configPath)
{
}

server::server(const std::string &host, const size_t &port, const unicore_config_t &info)
    : failed(false), listen_sockfd(-1), port(port), host(host) ,info(info)
{
    std::cout << "Server created for " << host << ":" << port << std::endl;
    failed = false;

    listen_sockfd = create_and_bind_socket(port, host.c_str());
    if (listen_sockfd < 0)
    {
        std::cerr << "Error creating socket for " << host << ":" << port << std::endl;
        failed = true;
        return;
    }
    if (listen(listen_sockfd, SOMAXCONN) < 0)
    {
        perror("listen");
        std::cerr << "Error listening on socket for " << host << ":" << port << std::endl;
        close(listen_sockfd);
        failed = true;
        return;
    }
}

int WebServer::init()
{
    std::vector<unicore_config_t> config;
    std::ifstream configFile(config_file);

    memset(events, 0, sizeof(events));

    if (!configFile.is_open())
    {
        std::cerr << "Error opening configuration file: " << config_file << std::endl;
        return 0;
    }

    if (unicore_config_parse(configFile, config) == -1)
    {
        std::cerr << "Error parsing configuration file: " << config_file << std::endl;
        return 0;
    }

    kq = kqueue();
    if (kq < 0)
    {
        std::cerr << "Error creating kqueue" << std::endl;
        return -1;
    }

    int i = 0;
    std::vector<unicore_config_t>::const_iterator it;
    for (it = config.begin(); it != config.end(); ++it)
    {
        const unicore_config_t &info = *it;
        std::string host = info.host;
        server srv =server(host, info.port, info);
        if (srv.failed)
        {
            std::cerr << "Failed to create server for " << host << ":" << info.port << std::endl;
            continue;
        }
        servers.push_back(srv);
        struct kevent event;
        EV_SET(&event, srv.listen_sockfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, &servers[i]);
        if (kevent(kq, &event, 1, nullptr, 0, nullptr) < 0)
        {
            std::cerr << "Error registering listen socket with kqueue" << std::endl;
            return -1;
        }
        i++;
    }
    return this->run();
}

WebServer::~WebServer()
{
}

server::~server()
{
}

bool WebServer::check_all_failed() const
{
    for (std::vector<server>::const_iterator it = servers.begin(); it != servers.end(); ++it)
    {
        if (!it->failed)
            return false;
    }
    return true;
}

int WebServer::run()
{
    if (servers.empty() || check_all_failed())
    {
        std::cerr << "No servers initialized. Exiting." << std::endl;
        return 0;
    }
    while (true)
    {
        int num_events = kevent(kq, nullptr, 0, events, 1024, nullptr);
        if (num_events < 0)
        {
            std::cerr << "Error in kevent" << std::endl;
            break;
        }

        for (int i = 0; i < num_events; ++i)
        {
            struct kevent &event = events[i];

            if (event.filter == EVFILT_READ)
            {
                server *srv = static_cast<server *>(event.udata);

                if (event.ident == static_cast<uintptr_t>(srv->listen_sockfd))
                {
                    srv->sockfd = accept(srv->listen_sockfd, nullptr, nullptr);
                    if (srv->sockfd < 0)
                    {
                        std::cerr << "Error accepting connection on " << srv->host << ":" << srv->port << std::endl;
                        continue;
                    }

                    int flags = fcntl(srv->sockfd, F_GETFL, 0);
                    fcntl(srv->sockfd, F_SETFL, flags | O_NONBLOCK);

                    struct kevent client_event;
                    EV_SET(&client_event, srv->sockfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, srv);
                    if (kevent(kq, &client_event, 1, nullptr, 0, nullptr) == -1)
                    {
                        std::cerr << "Failed to register client socket" << std::endl;
                        close(srv->sockfd);
                        continue;
                    }

                    std::cout << "Accepted connection on " << srv->host << ":" << srv->port << std::endl;
                }
                else if (event.ident == static_cast<uintptr_t>(srv->sockfd))
                {
                    std::cout << "Handling read event on fd " << event.ident << std::endl;
                    std::cerr << "Listen socket fd: " << srv->listen_sockfd << std::endl;
                    std::cerr << "Socket fd: " << srv->sockfd << std::endl;
                    std::cerr << "Server: " << srv->host << ":" << srv->port << std::endl;
                    unicore_buf_t buf_req;
                    char buf[4096];
                    buf_req.pos = ( u_char * )buf;
                    buf_req.start = buf_req.pos;

                    ssize_t bytes = recv(event.ident, buf, sizeof(buf), 0);
                    buf_req.end = buf_req.pos + bytes;
                    if (bytes <= 0)
                    {
                        if (bytes < 0)
                            std::cerr << "Read error on fd " << event.ident << std::endl;
                        else
                            std::cout << "Client disconnected on fd " << event.ident << std::endl;

                        close(event.ident);
                    }
                    else
                    {
                        // std::cout << "Received " << bytes << " bytes: ";
                        // std::cout.write(buf, bytes);
                        int req_line;
                        unicore_request_t request;
                        if ( srv->info.routes == NULL )
                        {
                            std::cerr << "bad";
                            std::exit(1);
                        }
                        req_line = unicore_http_parse_request_line ( &request , &buf_req , srv->info );
                        if (  req_line == 1 )
                        {

                            request.headers = new ht;
                            request.headers->buckets = new bucket [ M ];
                            std::memset ( request.headers->buckets , 0 , M );
                            if ( unicore_http_parse_field_lines ( &request , &buf_req ) == 1 )
                                std::cout << "parsed request-line and field-lines successfully" << std::endl;

                        }
                        http_response_t response;

                        response = build_http_response(request, req_line, 0, srv->info);
                        std::string response_str = format_http_response(response);
                        // std::cout << "Response: " << response_str << std::endl;
                        ssize_t sent_bytes = send(event.ident, response_str.c_str(), response_str.size(), 0);
                        if (sent_bytes < 0)
                        {
                            std::cerr << "Error sending response on fd " << event.ident << std::endl;
                        }
                        else
                        {
                            std::cout << "Sent " << sent_bytes << " bytes in response." << std::endl;
                        }



//request               
//rsponse
//send
                        std::cout << std::endl;
                        close(event.ident);
                        close(srv->sockfd);
                        srv->sockfd = -1;
                    }
                }
                else
                {
                    std::cerr << "Unhandled read event on fd " << event.ident << std::endl;
                    std::cerr << "Listen socket fd: " << srv->listen_sockfd << std::endl;
                    std::cerr << "Socket fd: " << srv->sockfd << std::endl;
                    std::cerr << "Server: " << srv->host << ":" << srv->port << std::endl;
                    close(event.ident);
                    close(srv->sockfd);
                    srv->sockfd = -1;
                    
                }
            }
            else
            {
                std::cerr << "Unhandled event type: " << event.filter << std::endl;
            }
        }
    }
    close(kq);
    return 0;
}
