#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
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
#include "../response/unicore_http_response.hpp"

class	server
{
	private:
		bool				running;

	public:
		int					sockfd;
		int					listen_sockfd;
		bool				failed;
		size_t				port;
		std::string			host;
		unicore_config_t	info;
		server(const std::string& host, const size_t& port, const unicore_config_t& info);
		server(const server& other);
		~server();
};

class   WebServer
{
	private:
		int					kq;
		struct kevent		events[1024];
		std::string			config_file;
		std::vector<server>	servers;
		bool				check_all_failed() const;

	public:
		WebServer(const std::string& config_file);
		~WebServer();
		int		init();
		int		run();
};

#endif
