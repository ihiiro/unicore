#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP
# define BUFFER_READ 6000000

# include "server.hpp"

class connection;

class   WebServer
{
	private:
		int					kq;
		std::string			config_file;
		struct kevent		events[10240];
		std::vector<server>	servers;
		std::map<int, connection *> connections;
		bool				check_all_failed() const;
		bool				server_already_exists(const std::string& host, size_t port) const;
        bool                has_same_name(const std::string &host, size_t port, const std::string &name) const;
		int					get_socket_fd(const std::string& host, size_t port) const;
		void				check_events_timeout();

	public:
		WebServer(const std::string& config_file);
		~WebServer();
		int		init();
		int		run();
};

#endif
