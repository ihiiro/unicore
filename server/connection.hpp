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
# include <signal.h>
# include "../core/unicore_http_parse.hpp"
# include "../core/unicore_defines.hpp"
# include "server.hpp"
#include "../response/unicore_http_response.hpp"
class server;

class connection
{
    protected:
        struct timeval                          last_activity;

    public:
        int                                     sockfd;
        std::string                             buffer;
        connection();
        connection(int fd);
		connection(const connection& other);
        virtual ~connection();

        void	update_last_activity();
		bool	has_timed_out() const;
        void	reset();
};

class server_conn : public connection
{
    public:
        int srv;

        server_conn();
        server_conn(int fd, int srv);
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
        int                 fdout;
        int                 cgi_pid;
        int                 request_line;
        bool                cgi_running;
        bool                chunked;
        bool                keep_alive;
        long				offset;
        std::string         rest;
		std::string 		filename;
        unicore_config_t    info;
		unicore_request_t	request;

        std::string&        getBuffer();
        client_conn(client_conn const& other);
        client_conn(int fd, const unicore_config_t& info, int request_line, unicore_request_t& request);
        ~client_conn();
};

#endif