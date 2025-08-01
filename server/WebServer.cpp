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
            std::cerr << "Server already exists for " << host << ":" << info.port << std::endl;
            return 0;
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
        connection *conn = connections[fd];
        connections.erase(fd);
        delete conn;
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
                    if (bytes <= 0)
                    {
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
                        if (conn->state.R == 2)
                        {
                            int valid = unicore_http_parse_message_body (conn->state , &buf_req);
                            if ( conn->info.redirection_list and get ( conn->info.redirection_list , conn->state.r->absolute_path ) and !( valid >= 400 and valid < 600 ) )
                                req_line = 1;
                            else if ( conn->state.r->REQUEST_METHOD == POST or ( valid >= 400 and valid < 600 ) )
                                req_line = valid;
                            else
                                req_line = 1;
                            if  (valid == 2)
                                std::cerr << "request not finished yet\n";
                            else
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
                        }
                        else
                        {
                            req_line = unicore_http_parse_request_line(conn->state, &buf_req, conn->info);
                            conn->state.redirect_guard = conn->info.redirection_list and get ( conn->info.redirection_list , conn->state.r->absolute_path );
                            if (req_line == 1)
                            {
                                int valid = 0;
                                if (unicore_http_parse_field_lines(conn->state , &buf_req) == 1)
                                    std::cerr << "parsed request-line and field-lines successfully" << std::endl;
                                valid = unicore_http_parse_message_body (conn->state , &buf_req);
                                if ( conn->info.redirection_list and get ( conn->info.redirection_list , conn->state.r->absolute_path ) and !( valid >= 400 and valid < 600 ) )
                                    req_line = 1;
                                else if ( conn->state.r->REQUEST_METHOD == POST or ( valid >= 400 and valid < 600 ))
                                req_line = valid;
                                else
                                    req_line = 1;
                                if  (valid == 2)
                                    std::cerr << "request not finished yet\n";
                                else
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
                            }
                            else if (req_line == 2)
                                std::cerr << "request not done yet" << std::endl;
                            else
                            {
                                std::cerr << "Error parsing request line on fd " << event.ident << std::endl;
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
            }
            else if (event.filter == EVFILT_WRITE)
            {
                client_conn *conn = dynamic_cast<client_conn *>(connections[event.ident]);
                if (!conn)
                {
                    std::cerr << "Unknown connection type for write event on fd " << event.ident << std::endl;
                    continue;
                }
                std::cerr << "Handling write event on fd " << event.ident << std::endl;
                if (conn->offset != -1337)
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
                    std::cerr << "Response not fully sent on fd " << event.ident << std::endl;
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
                    std::cerr << "Client disconnected while sending response on fd " << event.ident << std::endl;
                    struct kevent tmp_event;
                    EV_SET(&tmp_event, event.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
                    if (kevent(kq, &tmp_event, 1, NULL, 0, NULL) < 0)
                        std::cerr << "Error unregistering socket from kqueue" << std::endl;
                    close(event.ident);
                    connections.erase(event.ident);
                    delete conn;
                    continue ;
                }
                if (conn->offset == -1337 && conn->rest.size() == 0)
                {
                    if (!conn->keep_alive)
                    {
                        std::cerr << "Gracefully shutting down fd " << event.ident << std::endl;
                        shutdown(event.ident, SHUT_WR);

                        fd_set readfds;
                        struct timeval timeout;
                        FD_ZERO(&readfds);
                        FD_SET(event.ident, &readfds);
                        timeout.tv_sec = 1;
                        timeout.tv_usec = 0;

                        int ready = select(event.ident, &readfds, NULL, NULL, &timeout);
                        if (ready > 0 && FD_ISSET(event.ident, &readfds))
                        {
                            char tmp[1];
                            recv(event.ident, tmp, 1, 0);
                        }
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
                std::cerr << "Unhandled event type: " << event.filter << std::endl;
        }
        check_events_timeout();
    }
    close(kq);
    return 0;
}
