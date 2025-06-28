#include "connection.hpp"

<<<<<<< HEAD
=======
listening_conn::listening_conn()
    : connection(), info(), state()
{
    std::memset(&state, 0, sizeof(state));
}

listening_conn::listening_conn(int fd, const unicore_config_t& info)
    : connection(fd), info(info)
{
    // std::memset(&state, 0, sizeof(state));
    // state.r = new unicore_request_t;
    // state.r->headers = new ht;
    // state.r->headers->buckets = new bucket[M];
    // std::memset(state.r->headers->buckets, 0, M * sizeof(bucket));
}

listening_conn::listening_conn(const listening_conn& other)
    : connection(other.sockfd), info(other.info), state(other.state)
{
    if (other.state.r)
    {
        // state.r = new unicore_request_t;
        // *state.r = *other.state.r;
        // state.r->headers = new ht;
        // state.r->headers->buckets = new bucket[M];
        // std::memcpy(state.r->headers->buckets, other.state.r->headers->buckets, M * sizeof(bucket));
    }
    else
    {
        state.r = NULL;
    }
}

listening_conn::~listening_conn()
{
    if (state.r)
    {
        delete[] state.r->headers->buckets;
        delete state.r->headers;
        delete state.r;
    }
}

client_conn::client_conn(int fd)
    : connection(fd), offset(0)
{
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
}

connection::connection(int fd) : sockfd(fd)
{
}

connection::~connection()
{
}

void connection::reset()
{
    buffer.clear();
    sockfd = -1;
    std::cerr << "Connection reset on fd " << sockfd << std::endl;
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
>>>>>>> server_branch
