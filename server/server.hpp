#ifndef SERVER_HPP
# define SERVER_HPP
# define BUFFER_READ 50000

# include "connection.hpp"

class connection;

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
		std::string			config_file;
		struct kevent		events[10240];
		std::vector<server>	servers;
		std::map<int, connection *> connections;
		bool				connection_exists(const server& srv) const;
		bool				check_all_failed() const;

	public:
		WebServer(const std::string& config_file);
		~WebServer();
		int		init();
		int		run();
};

#endif
