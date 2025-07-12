#include "server/server.hpp"

int main(int argc, char *argv[])
{
    std::string	path;

	signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE to prevent crashes on broken pipes
    if (argc < 2)
		path = "default/default.conf";
	else if (argc == 2)
		path = argv[1];
	else
	{
		std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
		return 1;
	}
	WebServer webServer(path);
	if (!webServer.init())
	{
		std::cerr << "Web server failed" << std::endl;
		return 1;
	}

    return 0;
}
