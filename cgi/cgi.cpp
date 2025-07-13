#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <map>
#include "../core/unicore_request.hpp"

int execute_cgi(const unicore_request_t &req, std::string &result)
{
    int in_pipe[2];
    int out_pipe[2];
    char **env = NULL;

    if (pipe(in_pipe) < 0 || pipe(out_pipe) < 0)
    {
        perror("pipe");
        result = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        result = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        close(in_pipe[0]);
        close(in_pipe[1]);
        close(out_pipe[0]);
        close(out_pipe[1]);
        return -1;
    }
    else if (pid == 0)
    {
        dup2(in_pipe[0], STDIN_FILENO);
        dup2(out_pipe[1], STDOUT_FILENO);
        close(in_pipe[1]);
        close(out_pipe[0]);

        // Set environment variables
        env = new char *[10];
        int i = 0;
        env[i++] = (char *)"GATEWAY_INTERFACE=CGI/1.1";
        env[i++] = (char *)"SERVER_PROTOCOL=HTTP/1.1";
        if (req.REQUEST_METHOD == GET)
            env[i++] = (char *)"REQUEST_METHOD=GET";
        else if (req.REQUEST_METHOD == POST)
            env[i++] = (char *)"REQUEST_METHOD=POST";
        env[i++] = (char *)("SCRIPT_FILENAME=" + std::string((char *)req.PATH_TRANSLATED)).c_str();
        env[i++] = (char *)("PATH_INFO=" + std::string((char *)req.PATH_INFO)).c_str();
        env[i++] = (char *)("QUERY_STRING=" + std::string((char *)req.QUERY_STRING)).c_str();
        env[i++] = NULL;
        // Execute the CGI script

        char *args[] = {(char *)req.PATH_TRANSLATED, NULL};
        execve((char *)req.PATH_TRANSLATED, args, env);
        perror("execve");
        delete[] env;
        exit(1);
    }
    else
    {
        close(in_pipe[0]);
        close(out_pipe[1]);

        close(in_pipe[1]);

        char buffer[4096];
        ssize_t bytes;
        while ((bytes = read(out_pipe[0], buffer, sizeof(buffer))) > 0)
            result.append(buffer, bytes);
        close(out_pipe[0]);
        waitpid(pid, NULL, 0);
        return 1;
    }
}
