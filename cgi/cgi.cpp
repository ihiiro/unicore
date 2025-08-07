#include "cgi.hpp"

int monitor_cgi(client_conn *conn, std::string &result)
{
    int status;
    char buffer[80000];
    pid_t pid = waitpid(conn->cgi_pid, &status, WNOHANG);
    if (pid < 0)
    {
        perror("waitpid");
        result = "";
        return 500;
    }
    else
    {
        int bytes = read(conn->fdout, buffer, sizeof(buffer) - 1);
        if (bytes < 0)
        {
            perror("read");
            result = "";
            close(conn->fdout);
            conn->fdout = -1;
            if (WIFEXITED(status))
            {
                std::cerr << "CGI script exited with status: " << WEXITSTATUS(status) << std::endl;
            }
            else
            {
                std::cerr << "CGI script did not exit yet" << std::endl;
                kill(conn->cgi_pid, SIGKILL);
            }
            return 500;
        }
        else if (bytes == 0)
        {
            std::cerr << "CGI script has no output" << std::endl;
            close(conn->fdout);
            conn->fdout = -1;
            if (WIFEXITED(status))
            {
                std::cerr << "CGI script exited with status: " << WEXITSTATUS(status) << std::endl;
                return 500;
            }
            else
            {
                std::cerr << "CGI script did not exit yet" << std::endl;
                return -1337;
            }
        }
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) == 0)
            {
                while (bytes > 0)
                {
                    buffer[bytes] = '\0';
                    result += std::string(buffer, bytes);
                    bytes = read(conn->fdout, buffer, sizeof(buffer) - 1);
                }
                close(conn->fdout);
                conn->fdout = -1;
                return 200;
            }
            else
            {
                std::cerr << "CGI script exited with status: " << WEXITSTATUS(status) << std::endl;
                result = "";
                close(conn->fdout);
                conn->fdout = -1;
                return 500;
            }
        }
        else
        {
            std::cerr << "CGI script did not exit yet" << std::endl;
            return -1337;
        }
    }
}

int execute_cgi(unicore_request_t &req, std::string &result, client_conn *conn)
{
    int in_pipe[2];
    int out_pipe[2];
    int status;
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
    fcntl(out_pipe[0], F_SETFL, O_NONBLOCK);

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
        std::string path_translated = "./_ROOT_/" + std::string((char *)req.route->root) + std::string((char *)req.SCRIPT_NAME);

        env = new char *[8];
        int i = 0;
        env[i++] = (char *)"GATEWAY_INTERFACE=CGI/1.1";
        env[i++] = (char *)"SERVER_PROTOCOL=HTTP/1.1";
        if (req.REQUEST_METHOD == GET)
            env[i++] = (char *)"REQUEST_METHOD=GET";
        else if (req.REQUEST_METHOD == POST)
            env[i++] = (char *)"REQUEST_METHOD=POST";
        env[i++] = (char *)("SCRIPT_FILENAME=" + path_translated).c_str();
        if (req.PATH_INFO)
            env[i++] = (char *)("PATH_INFO=" + std::string((char *)req.PATH_INFO)).c_str();
        else
            env[i++] = (char *)"PATH_INFO=";
        if (req.QUERY_STRING)
            env[i++] = (char *)("QUERY_STRING=" + std::string((char *)req.QUERY_STRING)).c_str();
        else
            env[i++] = (char *)"QUERY_STRING=";
        env[i++] = (char *)("BODY=" + req.route->message_body).c_str();
        env[i++] = NULL;

        char **args;
        if (req.cgi_script_type == PYTHON)
        {
            args = new char *[3];
            args[0] = (char *)"python3";
            args[1] = (char *)path_translated.c_str();
            args[2] = NULL;
        }
        else
        {
            args = new char *[3];
            args[0] = (char *)"php";
            args[1] = (char *)path_translated.c_str();
            args[2] = NULL;    
        }
        execve((char *)args[1], args, env);
        perror("execve");
        delete[] env;
        exit(1);
    }
    else
    {
        close(in_pipe[0]);
        close(in_pipe[1]);
        close(out_pipe[1]);
        conn->cgi_pid = pid;
        conn->fdout = out_pipe[0];

        struct timeval start, now;
        gettimeofday(&start, NULL);

        double elapsed = 0.0;
        while (elapsed < 0.2)
        {
            waitpid(pid, &status, WNOHANG);
            gettimeofday(&now, NULL);
            elapsed = (now.tv_sec - start.tv_sec) + (now.tv_usec - start.tv_usec) / 1000000.0;
            usleep(10000);
        }
        char buffer[80000];
        ssize_t bytes;
        bytes = read(out_pipe[0], buffer, sizeof(buffer) - 1);
        std::cerr << "CGI script output: " << bytes << " bytes" << std::endl;
        if (bytes < 0)
        {
            perror("read");
            close(out_pipe[0]);
            result = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
            return 500;
        }
        else if (bytes == 0)
        {
            std::cerr << "CGI script has no output" << std::endl;
            close(out_pipe[0]);
            result = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
            return 500;
        }
        buffer[bytes] = '\0';
        result = std::string(buffer, bytes);
        if (WIFEXITED(status))
        {
            conn->cgi_running = false;
            if (WEXITSTATUS(status) == 0)
            {
                result = "HTTP/1.1 200 OK\r\n\r\n" + result;
                bytes = read(out_pipe[0], buffer, sizeof(buffer) - 1);
                while (bytes > 0)
                {
                    buffer[bytes] = '\0';
                    result += std::string(buffer, bytes);
                    bytes = read(out_pipe[0], buffer, sizeof(buffer) - 1);
                }
                close(out_pipe[0]);
                if (bytes < 0)
                {
                    perror("read");
                    result = "";
                    return 500;
                }
                return 200;
            }
            else
            {
                std::cerr << "CGI script exited with status: " << WEXITSTATUS(status) << std::endl;
                result = "";
                close(out_pipe[0]);
                return 500;
            }
        }
        conn->cgi_running = true;
        std::cerr << "CGI script did not exit yet" << std::endl;
        return -1337;
    }
}
