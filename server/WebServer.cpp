#include "WebServer.hpp"

WebServer::WebServer(const std::string &configPath) :  config_file(configPath)
{
}

bool WebServer::server_already_exists(const std::string &host, size_t port) const
{
    std::vector<server>::const_iterator it;
    for (it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->host == host && it->port == port)
            return true;
    }
    return false;
}

bool WebServer::has_same_name(const std::string &host, size_t port, const std::string &name) const
{
    std::vector<server>::const_iterator it;
    for (it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->host == host && it->port == port)
        {
            if (it->info.server_name == name)
                return true;
        }
    }
    return false;
}

int WebServer::get_socket_fd(const std::string &host, size_t port) const
{
    std::vector<server>::const_iterator it;
    for (it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->host == host && it->port == port)
        {
            return it->listen_sockfd;
        }
    }
    return -1;
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
        if (server_already_exists(host, info.port))
        {
            if (has_same_name(host, info.port, info.server_name))
            {
                std::cerr << "Server already exists for " << host << ":" << info.port << std::endl;
                return 0;
            }
            int fd = get_socket_fd(host, info.port);
            server srv(host, info.port, info, fd);
            continue;
        }
        server srv(host, info.port, info);
        if (srv.failed)
        {
            std::cerr << "Failed to create server for " << host << ":" << info.port << std::endl;
            continue;
        }
        servers.push_back(srv);
        unicore_route_t *r = (unicore_route_t *)get(servers[0].info.routes, (u_char *)"/")->value;
        struct kevent listen_event;
        connections[srv.listen_sockfd] = new server_conn(srv.listen_sockfd, &servers.back());
        EV_SET(&listen_event, srv.listen_sockfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, &connections[srv.listen_sockfd]);
        if (kevent(kq, &listen_event, 1, NULL, 0, NULL) < 0)
        {
            std::cerr << "Error registering listen socket with kqueue" << std::endl;
            return -1;
        }
        i++;
    }
    int result = this->run();
    return result;
}

WebServer::~WebServer()
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

void    WebServer::check_events_timeout()
{
    std::vector<int> to_remove;
    for (std::map<int, connection *>::iterator it = connections.begin(); it != connections.end(); ++it)
    {
        connection *conn = dynamic_cast<server_conn *>(it->second);
        if (conn)
            continue;
        else
            conn = it->second;
        if (conn->has_timed_out())
            to_remove.push_back(it->first);
    }
    for (std::vector<int>::iterator it = to_remove.begin(); it != to_remove.end(); ++it)
    {
        int fd = *it;
        std::cerr << "Removing connection " << fd << " due to timeout" << std::endl;
        struct kevent tmp_event;
        EV_SET(&tmp_event, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
            std::cerr << "Error unregistering socket from kqueue" << std::endl;
        connections.erase(fd);
        close(fd);
    }
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
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 10000000;
        int num_events = kevent(kq, NULL, 0, events, 10240, &ts);
        if (num_events < 0)
        {
            std::cerr << "Error in kevent" << std::endl;
            continue ;
        }
        for (int i = 0; i < num_events; i++)
        {
            std::cerr << "Event " << i + 1 << " of " << num_events << std::endl;
            struct kevent &event = events[i];

            if (event.filter == EVFILT_READ)
            {
                connection &gen_conn = *connections[event.ident];
                server_conn *conn = dynamic_cast<server_conn *>(&gen_conn);
                if (conn)
                {

                    server *srv = conn->srv;
                    
                    if (event.ident == static_cast<uintptr_t>(srv->listen_sockfd))
                    {
                        srv->sockfd = accept(srv->listen_sockfd, NULL, NULL);
                        if (srv->sockfd < 0)
                        {
                            std::cerr << "Error accepting connection on " << srv->host << ":" << srv->port << std::endl;
                            continue;
                        }

                        int flags = fcntl(srv->sockfd, F_GETFL, 0);
                        fcntl(srv->sockfd, F_SETFL, flags | O_NONBLOCK);

                        struct kevent client_event;
                        connections[srv->sockfd] = new listening_conn(srv->sockfd, srv->info);
                        EV_SET(&client_event, srv->sockfd, EVFILT_READ, EV_ADD | EV_ENABLE , 0, 0, connections[srv->sockfd]);
                        if (kevent(kq, &client_event, 1, NULL, 0, NULL) == -1)
                        {
                            std::cerr << "Failed to register client socket" << std::endl;
                            close(srv->sockfd);
                            continue;
                        }

                        std::cerr << "Accepted connection on " << srv->host << ":" << srv->port << std::endl;
                    }
                }
                else
                {
                    listening_conn *conn = dynamic_cast<listening_conn *>(&gen_conn);
                    if (!conn)
                    {
                        std::cerr << "Unknown connection type for fd " << event.ident << std::endl;
                        continue;
                    }
                    std::cerr << "Handling read event on fd " << event.ident << std::endl;
                    unicore_buf_t buf_req;
                    char buf[BUFFER_READ];
                    std::memset(buf, 0, sizeof(buf));
                    ssize_t bytes;
                    bytes = recv(event.ident, buf, BUFFER_READ, 0);
                    conn->update_last_activity();
                    if (bytes > BUFFER_READ)
                    {
                        std::cerr << "Received too much data on fd " << event.ident << std::endl;
                        close(event.ident);
                        break;
                    }
                    buf_req.pos = (u_char *)buf;
                    buf_req.start = buf_req.pos;
                    buf_req.end = buf_req.start + bytes - 1;
                    // write ( 2 , buf , bytes );
                    if (bytes <= 0)
                    {
                        std::cerr << "Client disconnected or error on fd " << event.ident << std::endl;
                        if (bytes < 0)
                            std::cerr << "Read error on fd " << event.ident << std::endl;
                        else
                            std::cerr << "Client disconnected on fd " << event.ident << std::endl;
                        struct kevent tmp_event;
                        EV_SET(&tmp_event, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                        if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                        {
                            std::cerr << "Error unregistering socket from kqueue" << std::endl;
                        }
                        close(event.ident);
                        connections.erase(event.ident);
                    }
                    else
                    {
                        int req_line;
                        if (conn->info.routes == NULL)
                        {
                            std::cerr << "\033[31mNo routes defined for server " << conn->info.server_name << "\033[0m" << std::endl;
                            exit(1);
                        }
                        if (conn->state.R == 2)
                        {
                            int valid = unicore_http_parse_message_body(conn->state , &buf_req);
                            if  (valid == 2)
                                std::cerr << "request not finished yet 1\n";
                            else if (valid == 1)
                            {
                                std::cerr << "request finished\n";
                                connections.erase(event.ident);
                                connections[event.ident] = new client_conn(event.ident, conn->info, req_line, *conn->state.r);
                                delete conn;
                                struct kevent tmp_event;
                                EV_SET(&tmp_event, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                                if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                                    std::cerr << "Error unregistering read event for request" << std::endl;
                                EV_SET(&tmp_event, event.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, connections[event.ident]);
                                if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                                    std::cerr << "Error registering write event for request" << std::endl;
                            }
                            else
                            {
                                std::cerr << "bad message body\n";
                                connections.erase(event.ident);
                                connections[event.ident] = new client_conn(event.ident, conn->info, 400, *conn->state.r);
                                delete conn;
                                struct kevent tmp_event;
                                EV_SET(&tmp_event, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                                if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                                    std::cerr << "Error unregistering read event for request" << std::endl;
                                EV_SET(&tmp_event, event.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, connections[event.ident]);
                                if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                                    std::cerr << "Error registering write event for request" << std::endl;
                            }
                        }
                        else
                        {
                            req_line = unicore_http_parse_request_line(conn->state, &buf_req, conn->info);
                            if (req_line == 1)
                            {
                                std::cout << "fffff\n";
                                int valid = 0;
                                if (unicore_http_parse_field_lines(conn->state , &buf_req) == 1)
                                    std::cerr << "parsed request-line and field-lines successfully" << std::endl;
                                valid = unicore_http_parse_message_body(conn->state , &buf_req);
                                if  (valid == 2)
                                    std::cerr << "request not finished yet 2\n";
                                else if (valid == 1)
                                {
                                    std::cerr << "request finished bottom\n";
                                    connections.erase(event.ident);
                                    connections[event.ident] = new client_conn(event.ident, conn->info, req_line, *conn->state.r);
                                    delete conn;
                                    struct kevent tmp_event;
                                    EV_SET(&tmp_event, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                                    if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                                        std::cerr << "Error unregistering read event for request" << std::endl;
                                    EV_SET(&tmp_event, event.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, connections[event.ident]);
                                    if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                                        std::cerr << "Error registering write event for request" << std::endl;
                                }
                                else
                                {
                                    std::cerr << "bad message body " << valid << std::endl;
                                    connections.erase(event.ident);
                                    connections[event.ident] = new client_conn(event.ident, conn->info, 400, *conn->state.r);
                                    delete conn;
                                    struct kevent tmp_event;
                                    EV_SET(&tmp_event, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                                    if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                                        std::cerr << "Error unregistering read event for request" << std::endl;
                                    EV_SET(&tmp_event, event.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, connections[event.ident]);
                                    if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                                        std::cerr << "Error registering write event for request" << std::endl;
                                }
                            }
                            else if (req_line == 2)
                                std::cerr << "response not done yet" << std::endl;
                            else
                            {
                                std::cerr << "bad request line" << std::endl;
                                connections.erase(event.ident);
                                connections[event.ident] = new client_conn(event.ident, conn->info, req_line, *conn->state.r);
                                delete conn;
                                struct kevent tmp_event;
                                EV_SET(&tmp_event, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                                if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                                    std::cerr << "Error unregistering read event for request" << std::endl;
                                EV_SET(&tmp_event, event.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, connections[event.ident]);
                                if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                                    std::cerr << "Error registering write event for request" << std::endl;
                            }
                        }
                    }
                }
                std::cerr << "event filter is: " << event.filter << std::endl;
            }
            else if (event.filter == EVFILT_WRITE)
            {
                client_conn *conn = dynamic_cast<client_conn *>(connections[event.ident]);
                if (!conn)
                {
                    std::cerr << "Unknown connection type for write event on fd " << event.ident << std::endl;
                    continue;
                }
                std::cerr << "Handling correct write event on fd " << event.ident << std::endl;
                std::cerr << "Filename: " << conn->filename << std::endl;
                build_http_response(*conn, conn->request_line);
                std::string response = "";
                if (conn->rest.size() > 0)
                {
                    response += conn->rest;
                    conn->rest.clear();
                }
                response += conn->getBuffer();
                std::cout << "response is " << response << std::endl;
                size_t bytes_sent = 0;
                size_t total_size = response.size(); 
                bytes_sent = send(event.ident, response.c_str(), total_size, 0);
                if (bytes_sent < 0 || bytes_sent < total_size)
                {
                    std::cerr << "Response not fully sent on fd " << event.ident << ": " << strerror(errno) << std::endl;
                    if (bytes_sent < 0)
                        conn->rest = response;
                    else
                        conn->rest = response.substr(bytes_sent);
                }
                else if (bytes_sent == total_size)
                {
                    std::cerr << "Response fully sent on fd " << event.ident << std::endl;
                    conn->update_last_activity();
                }
                else
                {
                    std::cerr << "Unexpected bytes sent on fd " << event.ident << ": " << bytes_sent << std::endl;
                }
                if (conn->offset == -1337)
                {
                    if (!conn->keep_alive)
                    {
                        std::cerr << "Closing connection on fd " << event.ident << std::endl;
                        struct kevent tmp_event;
                        connections.erase(event.ident);
                        delete conn;
                        EV_SET(&tmp_event, event.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
                        if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                            std::cerr << "Error unregistering socket from kqueue" << std::endl;
                        close(event.ident);
                    }
                    else
                    {
                        std::cerr << "Keep-alive connection on fd " << event.ident << std::endl;
                        connections.erase(event.ident);
                        connections[event.ident] = new listening_conn(event.ident, conn->info);
                        delete conn;
                        struct kevent tmp_event;
                        EV_SET(&tmp_event, event.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
                        if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                            std::cerr << "Error unregistering write event for keep-alive connection" << std::endl;
                        EV_SET(&tmp_event, event.ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, connections[event.ident]);
                        if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                            std::cerr << "Error re-registering read event for keep-alive connection" << std::endl;
                    }
                }
                else
                    std::cerr << "Response sent on fd " << event.ident << std::endl;
            }
            else
            {
                std::cerr << "Unhandled event type: " << event.filter << std::endl;
            }
        }
        check_events_timeout();
    }
    close(kq);
    return 0;
}
