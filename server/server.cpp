#include "server.hpp"

server::server(const server &other)
    : sockfd(other.sockfd), listen_sockfd(other.listen_sockfd), failed(other.failed),
      port(other.port), host(other.host), info(other.info)
{
}

void server::create_and_bind_socket()
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        listen_sockfd = -1;
    }

    int opt = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0 && host != "localhost")
    {
        std::cerr << "Invalid IP address: " << host << std::endl;
        close(socketfd);
        listen_sockfd = -1;
    }
    if (host == "localhost")
    {
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }

    if (fcntl(socketfd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "Error setting socket to non-blocking mode" << std::endl;
        close(socketfd);
        listen_sockfd = -1;
    }
    
    if (bind(socketfd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Error binding socket to port " << port << std::endl;
        std::cerr << "Error: " << strerror(errno) << std::endl;
        close(socketfd);
        listen_sockfd = -1;
    }
    
    listen_sockfd = socketfd;
}

server::server(const std::string &host, const size_t &port, const unicore_config_t &info)
    : listen_sockfd(-1), failed(false), port(port), host(host) ,info(info)
{
    std::cerr << "Server created for " << host << ":" << port << std::endl;
    failed = false;

    create_and_bind_socket();
    if (listen_sockfd < 0)
    {
        std::cerr << "Error creating socket for " << host << ":" << port << std::endl;
        failed = true;
        return;
    }
    if (listen(listen_sockfd, SOMAXCONN) < 0)
    {
        perror("listen");
        std::cerr << "Error listening on socket for " << host << ":" << port << std::endl;
        close(listen_sockfd);
        failed = true;
        return;
    }
}

server::~server()
{
}
