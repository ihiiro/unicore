#include "connection.hpp"

listening_conn::listening_conn()
    : connection(), info()
{
    std::memset(&state, 0, sizeof( fsm_state_t ));
    state.mimes->buckets = new bucket [ M ];
    std::memset ( state.mimes->buckets , 0 , M * sizeof ( bucket ) );
    insert ( state.mimes , ( u_char * )"audio/aac" , ( char * )".aac" );
    insert ( state.mimes , ( u_char * )"image/apng" , ( char * )".apng" );
    insert ( state.mimes , ( u_char * )"application/x-freearc" , ( char * )".arc" );
    insert ( state.mimes , ( u_char * )"image/avif" , ( char * )".avif" );
    insert ( state.mimes , ( u_char * )"video/x-msvideo" , ( char * )".avi" );
    insert ( state.mimes , ( u_char * )"application/octet-stream" , ( char * )".bin" );
    insert ( state.mimes , ( u_char * )"image/bmp" , ( char * )".bmp" );
    insert ( state.mimes , ( u_char * )"text/css" , ( char * )".css" );
    insert ( state.mimes , ( u_char * )"text/csv" , ( char * )".csv" );
    insert ( state.mimes , ( u_char * )"application/epub+zip" , ( char * )".epub" );
    insert ( state.mimes , ( u_char * )"image/gif" , ( char * )".gif" );
    insert ( state.mimes , ( u_char * )"text/html" , ( char * )".html" );
    insert ( state.mimes , ( u_char * )"image/jpeg" , ( char * )".jpg" );
    insert ( state.mimes , ( u_char * )"text/markdown" , ( char * )".md" );
    insert ( state.mimes , ( u_char * )"audio/mpeg" , ( char * )".mp3" );
    insert ( state.mimes , ( u_char * )"video/mp4" , ( char * )".mp4" );
    insert ( state.mimes , ( u_char * )"video/mpeg" , ( char * )".mpeg" );
    insert ( state.mimes , ( u_char * )"image/png" , ( char * )".png" );
    insert ( state.mimes , ( u_char * )"application/pdf" , ( char * )".pdf" );
    insert ( state.mimes , ( u_char * )"image/svg+xml" , ( char * )".svg" );
    insert ( state.mimes , ( u_char * )"video/mp2t" , ( char * )".ts" );
    insert ( state.mimes , ( u_char * )"audio/wav" , ( char * )".wav" );
    insert ( state.mimes , ( u_char * )"audio/webm" , ( char * )".weba" );
    insert ( state.mimes , ( u_char * )"video/webm" , ( char * )".webm" );
    insert ( state.mimes , ( u_char * )"image/webp" , ( char * )".webp" );
    insert ( state.mimes , ( u_char * )"application/xhtml+xml" , ( char * )".xhtml" );
    insert ( state.mimes , ( u_char * )"application/xml" , ( char * )".xml" );
    state.R = 0;
    state.file = new std::ofstream;
}

listening_conn::listening_conn(int fd, const unicore_config_t& info)
    : connection(fd), info(info)
{
    std::memset(&state, 0, sizeof( fsm_state_t ));
    state.mimes = new ht;
    state.mimes->buckets = new bucket [ M ];
    std::memset ( state.mimes->buckets , 0 , M * sizeof ( bucket ) );
    insert ( state.mimes , ( u_char * )"audio/aac" , ( char * )".aac" );
    insert ( state.mimes , ( u_char * )"image/apng" , ( char * )".apng" );
    insert ( state.mimes , ( u_char * )"application/x-freearc" , ( char * )".arc" );
    insert ( state.mimes , ( u_char * )"image/avif" , ( char * )".avif" );
    insert ( state.mimes , ( u_char * )"video/x-msvideo" , ( char * )".avi" );
    insert ( state.mimes , ( u_char * )"application/octet-stream" , ( char * )".bin" );
    insert ( state.mimes , ( u_char * )"image/bmp" , ( char * )".bmp" );
    insert ( state.mimes , ( u_char * )"text/css" , ( char * )".css" );
    insert ( state.mimes , ( u_char * )"text/csv" , ( char * )".csv" );
    insert ( state.mimes , ( u_char * )"application/epub+zip" , ( char * )".epub" );
    insert ( state.mimes , ( u_char * )"image/gif" , ( char * )".gif" );
    insert ( state.mimes , ( u_char * )"text/html" , ( char * )".html" );
    insert ( state.mimes , ( u_char * )"image/jpeg" , ( char * )".jpg" );
    insert ( state.mimes , ( u_char * )"text/markdown" , ( char * )".md" );
    insert ( state.mimes , ( u_char * )"audio/mpeg" , ( char * )".mp3" );
    insert ( state.mimes , ( u_char * )"video/mp4" , ( char * )".mp4" );
    insert ( state.mimes , ( u_char * )"video/mpeg" , ( char * )".mpeg" );
    insert ( state.mimes , ( u_char * )"image/png" , ( char * )".png" );
    insert ( state.mimes , ( u_char * )"application/pdf" , ( char * )".pdf" );
    insert ( state.mimes , ( u_char * )"image/svg+xml" , ( char * )".svg" );
    insert ( state.mimes , ( u_char * )"video/mp2t" , ( char * )".ts" );
    insert ( state.mimes , ( u_char * )"audio/wav" , ( char * )".wav" );
    insert ( state.mimes , ( u_char * )"audio/webm" , ( char * )".weba" );
    insert ( state.mimes , ( u_char * )"video/webm" , ( char * )".webm" );
    insert ( state.mimes , ( u_char * )"image/webp" , ( char * )".webp" );
    insert ( state.mimes , ( u_char * )"application/xhtml+xml" , ( char * )".xhtml" );
    insert ( state.mimes , ( u_char * )"application/xml" , ( char * )".xml" );
    state.R = 0;
    state.file = new std::ofstream;
}

listening_conn::listening_conn(const listening_conn& other)
    : connection(other.sockfd), info(other.info)
{
    std::memset(&state, 0, sizeof(fsm_state_t));
    state.mimes = new ht;
    state.mimes->buckets = new bucket [ M ];
    std::memset ( state.mimes->buckets , 0 , M * sizeof ( bucket ) );
    insert ( state.mimes , ( u_char * )"audio/aac" , ( char * )".aac" );
    insert ( state.mimes , ( u_char * )"image/apng" , ( char * )".apng" );
    insert ( state.mimes , ( u_char * )"application/x-freearc" , ( char * )".arc" );
    insert ( state.mimes , ( u_char * )"image/avif" , ( char * )".avif" );
    insert ( state.mimes , ( u_char * )"video/x-msvideo" , ( char * )".avi" );
    insert ( state.mimes , ( u_char * )"application/octet-stream" , ( char * )".bin" );
    insert ( state.mimes , ( u_char * )"image/bmp" , ( char * )".bmp" );
    insert ( state.mimes , ( u_char * )"text/css" , ( char * )".css" );
    insert ( state.mimes , ( u_char * )"text/csv" , ( char * )".csv" );
    insert ( state.mimes , ( u_char * )"application/epub+zip" , ( char * )".epub" );
    insert ( state.mimes , ( u_char * )"image/gif" , ( char * )".gif" );
    insert ( state.mimes , ( u_char * )"text/html" , ( char * )".html" );
    insert ( state.mimes , ( u_char * )"image/jpeg" , ( char * )".jpg" );
    insert ( state.mimes , ( u_char * )"text/markdown" , ( char * )".md" );
    insert ( state.mimes , ( u_char * )"audio/mpeg" , ( char * )".mp3" );
    insert ( state.mimes , ( u_char * )"video/mp4" , ( char * )".mp4" );
    insert ( state.mimes , ( u_char * )"video/mpeg" , ( char * )".mpeg" );
    insert ( state.mimes , ( u_char * )"image/png" , ( char * )".png" );
    insert ( state.mimes , ( u_char * )"application/pdf" , ( char * )".pdf" );
    insert ( state.mimes , ( u_char * )"image/svg+xml" , ( char * )".svg" );
    insert ( state.mimes , ( u_char * )"video/mp2t" , ( char * )".ts" );
    insert ( state.mimes , ( u_char * )"audio/wav" , ( char * )".wav" );
    insert ( state.mimes , ( u_char * )"audio/webm" , ( char * )".weba" );
    insert ( state.mimes , ( u_char * )"video/webm" , ( char * )".webm" );
    insert ( state.mimes , ( u_char * )"image/webp" , ( char * )".webp" );
    insert ( state.mimes , ( u_char * )"application/xhtml+xml" , ( char * )".xhtml" );
    insert ( state.mimes , ( u_char * )"application/xml" , ( char * )".xml" );
    state.file = new std::ofstream;
    state.R = 0;
}

listening_conn::~listening_conn()
{
    delete state.r;
    delete state.file;
    delete[] state.mimes->buckets;
    delete state.mimes;
}

client_conn::client_conn(int fd, const unicore_config_t& info, int request_line, unicore_request_t& request)
    : connection(fd), offset(0), filename(""), info(info), request_line(request_line), request(request), chunked(false)
{
}

client_conn::client_conn(client_conn const& other)
    : connection(other.sockfd), offset(other.offset), filename(other.filename),
      info(other.info), request_line(other.request_line), request(other.request),
      chunked(other.chunked), rest(other.rest)
{
    buffer = other.buffer;
}

std::string& client_conn::getBuffer()
{
    return buffer;
}

client_conn::~client_conn()
{
    for (int i = 0; i < M; i++)
    {
        delete request.headers->buckets[i].key;
        delete (char *)request.headers->buckets[i].value;
    }
    delete[] request.headers->buckets;
    delete request.headers;
    delete request.PATH_INFO;
    delete request.SCRIPT_NAME;
    delete request.QUERY_STRING;
    delete request.absolute_path;
    delete request.static_uri_path;
    delete request.PATH_TRANSLATED;
    delete request.GATEWAY_INTERFACE;
}

connection::connection() : sockfd(-1)
{
    gettimeofday(&last_activity, NULL);
    buffer.clear();
}

connection::connection(int fd) : sockfd(fd)
{
    gettimeofday(&last_activity, NULL);
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
    gettimeofday(&last_activity, NULL);
}

bool connection::has_timed_out() const
{
    struct timeval  now;

    gettimeofday(&now, NULL);
    double elapsed = (now.tv_sec - last_activity.tv_sec) + (now.tv_usec - last_activity.tv_usec) / 1000000.0;
    return elapsed > 30.0;
}

void connection::reset()
{
    std::cerr << "Connection reset on fd " << sockfd << std::endl;
    buffer.clear();
    gettimeofday(&last_activity, NULL);
}
