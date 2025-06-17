#include "server.hpp"

WebServer::WebServer(const std::string &configPath) :  config_file(configPath)
{
}

int WebServer::init()
{
    std::cout << "Initializing WebServer with configuration file: " << config_file << std::endl;
    std::vector<unicore_config_t> config;
    std::ifstream configFile(config_file);

    if (!configFile.is_open())
    {
        std::cerr << "Error opening configuration file: " << config_file << std::endl;
        return 0;
    }

    if (!unicore_config_parse(configFile, config))
    {
        std::cerr << "Error parsing configuration file: " << config_file << std::endl;
        return 0;
    }

    std::cout << "Configuration loaded successfully." << std::endl;

    std::vector<unicore_config_t>::const_iterator it;
    for (it = config.begin(); it != config.end(); ++it)
    {
        std::cout << "Config Entry: " << it->host << " = " << it->port << std::endl;
    }

    return 1;
}

void WebServer::run()
{
    std::cout << "WebServer is running..." << std::endl;
    // Here you would typically start the server loop, accepting connections, etc.
}

void WebServer::stop()
{
    std::cout << "WebServer is stopping..." << std::endl;
    // Here you would typically clean up resources, close sockets, etc.
}

WebServer::~WebServer()
{
    std::cout << "WebServer is being destroyed..." << std::endl;
    // Here you would typically clean up resources, close sockets, etc.
}

server::server(const std::string &host, const std::string &port, const unicore_config_t &info)
    : host(host), port(port), info(info), running(false)
{
    // Initialize the server socket here
    std::cout << "Server created for " << host << ":" << port << std::endl;
}

server::~server()
{
    std::cout << "Server for " << host << ":" << port << " is being destroyed..." << std::endl;
    // Clean up resources, close sockets, etc.
}

void server::run()
{
    running = true;
    std::cout << "Server running on " << host << ":" << port << std::endl;
    // Here you would typically start the server loop, accepting connections, etc.
}

void server::stop()
{
    running = false;
    std::cout << "Server on " << host << ":" << port << " is stopping..." << std::endl;
    // Here you would typically clean up resources, close sockets, etc.
}


