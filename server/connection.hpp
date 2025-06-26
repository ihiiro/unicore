#ifndef CONNECTIONS_HPP
# define CONNECTIONS_HPP

# include <string>
# include <map>
# include <memory>
# include <iostream>
# include <cstring>
# include <fcntl.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <arpa/inet.h>
# include "../core/unicore_http_parse.hpp"
# include "../core/unicore_defines.hpp"
# include "../response/unicore_http_response.hpp"
# include "server.hpp"

class server;

class connection
{
    protected:
        int         sockfd;
        std::string buffer;

    public:
        connection();
        connection(int fd);
        virtual ~connection();

        void reset();
};

class server_conn : public connection
{
    public:
        server	*srv;

        server_conn();
        server_conn(int fd, server* srv);
        server_conn(const server_conn& other);
        ~server_conn();
};

class listening_conn : public connection
{
    public:
        fsm_state_t         state;
        unicore_config_t    info;
        listening_conn();
        listening_conn(int fd, const unicore_config_t& info);
        listening_conn(const listening_conn& other);
        ~listening_conn();
};

class client_conn : public connection
{
    public:
        long				offset;
		std::string 		filename;
		unicore_request_t	request;

        client_conn(int fd);
        ~client_conn();
};

#endif