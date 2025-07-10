#include "connection.hpp"

listening_conn::listening_conn()
    : connection(), info(), state()
{
    std::memset(&state, 0, sizeof(state));
}

listening_conn::listening_conn(int fd, const unicore_config_t& info)
    : connection(fd), info(info)
{
    std::memset(&state, 0, sizeof(state));
}

listening_conn::listening_conn(const listening_conn& other)
    : connection(other.sockfd), info(other.info), state(other.state)
{
    if (!other.state.r)
        state.r = NULL;
}

listening_conn::~listening_conn()
{
    if (state.r)
    {
        // delete[] state.r->headers->buckets;
        // delete state.r->headers;
        // delete state.r;
    }
}

client_conn::client_conn(int fd, const unicore_config_t& info, int request_line, unicore_request_t& request)
    : connection(fd), offset(0), filename(""), info(info), request_line(request_line), request(request), chunked(false)
{
}

std::string& client_conn::getBuffer()
{
    return buffer;
}

client_conn::~client_conn()
{
    if (request.headers)
    {
        delete[] request.headers->buckets;
        delete request.headers;
    }
}

connection::connection() : sockfd(-1)
{
    last_activity = std::chrono::steady_clock::now();
    buffer.clear();
}

connection::connection(int fd) : sockfd(fd)
{
    last_activity = std::chrono::steady_clock::now();
    buffer.clear();
}

connection::connection(const connection& other)
    : sockfd(other.sockfd), buffer(other.buffer), last_activity(other.last_activity)
{
}

connection::~connection()
{
}

server_conn::server_conn() : connection(), srv(nullptr)
{
}

server_conn::server_conn(int fd, server* srv)
    : connection(fd), srv(srv)
{
}

server_conn::server_conn(const server_conn& other)
    : connection(other.sockfd), srv(other.srv)
{
}

server_conn::~server_conn()
{
}

void connection::update_last_activity()
{
    last_activity = std::chrono::steady_clock::now();
}

bool connection::has_timed_out() const
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = now - last_activity;
    return duration.count() > 10;
}

void connection::reset()
{
    std::cerr << "Connection reset on fd " << sockfd << std::endl;
    buffer.clear();
    last_activity = std::chrono::steady_clock::now();
}
