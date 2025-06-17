#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <memory>
# include <iostream>
# include "../core/unicore_http_parse.hpp"
# include "../core/unicore_defines.hpp"

class	server
{
	private:
		std::string			host;
		std::string			port;
		int					sockfd;
		bool				running;
		unicore_config_t	info;

	public:
		server(const std::string& host, const std::string& port, const unicore_config_t& info);
		~server();
		void    run();
		void    stop();
};

class   WebServer
{
	private:
		std::string		config_file;
		std::vector<server>	servers;

	public:
		WebServer(const std::string& config_file);
		~WebServer();
		int		init();
		void    run();
		void    stop();
};

#endif
