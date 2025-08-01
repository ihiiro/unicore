#include "cgi.hpp"

int execute_cgi(unicore_request_t &req, std::string &result)
{
    static int cgi_count = 0;
    int in_pipe[2];
    int out_pipe[2];
    char **env = NULL;

    if (pipe(in_pipe) < 0 || pipe(out_pipe) < 0)
    {
        perror("pipe");
        result = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        if (in_pipe[0] >= 0)
            close(in_pipe[0]);
        if (in_pipe[1] >= 0)
            close(in_pipe[1]);
        return 500;
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
        return 500;
    }
    else if (pid == 0)
    {
        dup2(in_pipe[0], STDIN_FILENO);
        dup2(out_pipe[1], STDOUT_FILENO);
        close(in_pipe[1]);
        close(out_pipe[0]);
        std::string path_translated = "." + std::string((char *)req.route->root) + std::string((char *)req.SCRIPT_NAME);

        env = new char *[10];
        int i = 0;
        env[i++] = (char *)"GATEWAY_INTERFACE=CGI/1.1";
        env[i++] = (char *)"SERVER_PROTOCOL=HTTP/1.1";
        if (req.REQUEST_METHOD == GET)
            env[i++] = (char *)"REQUEST_METHOD=GET";
        else if (req.REQUEST_METHOD == POST)
            env[i++] = (char *)"REQUEST_METHOD=POST";
        env[i++] = (char *)("SCRIPT_FILENAME=" + path_translated).c_str();
        env[i++] = (char *)("PATH_INFO=" + std::string((char *)req.PATH_INFO)).c_str();
        env[i++] = (char *)("QUERY_STRING=" + std::string((char *)req.QUERY_STRING)).c_str();
        env[i++] = NULL;

        char **args;
        if (req.cgi_script_type == PYTHON)
        {
            args = new char *[3];
            args[0] = (char *)"python";
            args[1] = (char *)path_translated.c_str();
            args[2] = NULL;
        }
        else if (req.cgi_script_type == PHP && req.route->CGI_PHP)
        {
            args = new char *[3];
            args[0] = (char *)"php";
            args[1] = (char *)path_translated.c_str();
            args[2] = NULL;    
        }
        std::cerr << "Executing CGI script: " << path_translated << std::endl;
        execve((char *)path_translated.c_str(), args, env);
        perror("execve");
        delete[] env;
        exit(1);
    }
    else
    {
        close(in_pipe[0]);
        close(in_pipe[1]);
        close(out_pipe[1]);

        struct timeval start, now;
        gettimeofday(&start, NULL);

        double elapsed = 0.0;
        while (elapsed < 5.0)
        {
            waitpid(pid, NULL, WNOHANG);
            gettimeofday(&now, NULL);
            elapsed = (now.tv_sec - start.tv_sec) + (now.tv_usec - start.tv_usec) / 1000000.0;
        }
        char buffer[4096];
        ssize_t bytes;
        result.append("HTTP/1.1 200 OK\r\n");
        while ((bytes = read(out_pipe[0], buffer, sizeof(buffer))) > 0)
            result.append(buffer, bytes);
        close(out_pipe[0]);
        return 1;
    }
}
