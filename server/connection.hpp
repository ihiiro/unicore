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

class connection
{
    protected:
        int         sockfd;
        std::string buffer;

        connection(int fd);
        ~connection();

        void reset();
};

class listening_conn : public connection
{
    public:
        unicore_request_t request;
        unicore_config_t info;
        listening_conn(int fd, const unicore_config_t& info);
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