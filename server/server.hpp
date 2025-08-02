#ifndef SERVER_HPP
# define SERVER_HPP

# include "connection.hpp"

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
		void create_and_bind_socket();
};

#endif
