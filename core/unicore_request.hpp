

#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <cstring>
#include <fstream>

/* HASH TABLE */
#ifndef FNV_PRIME
 #define FNV_PRIME 1099511628211UL
#endif

#ifndef FNV_OFFSET_BASIS
 #define FNV_OFFSET_BASIS 14695981039346656037UL
#endif

#ifndef PROBE_LENGTH
 #define PROBE_LENGTH 1
#endif

#ifndef M
 #define M 200
#endif

typedef struct
{

    const u_char     *key;
    void             *value;

} bucket;

typedef struct 
{

    bucket *buckets;
    
} ht;

uint64_t FNV_1A_HASH ( const u_char *k );
uint64_t hash ( const u_char *k );
void insert ( ht *HT , const u_char *key , void *value );
bucket *get ( ht *HT , const u_char *key );

typedef std::ifstream::traits_type traits_type;

typedef struct
{

    char            *root;
    char            *file_if_directory_request;
    char            *upload_path;

    bool            ROUTE_GET;
    bool            ROUTE_POST;
    bool            ROUTE_DELETE;
    bool            ROUTE_UPLOADS;

    bool            CGI_GET;
    bool            CGI_POST;
    bool            CGI_PYTHON;
    bool            CGI_PHP;

    bool            directory_listing;


} unicore_route_t;

// GET /ROOT/(PCHAR/PCHAR...)[ "?" query ] HTTP/1.1
// upper case fields are CGI/1.1 variables (RFC 3875)
typedef struct
{

    ht              *headers;           /* hash table by field name */
    u_char          *static_uri_path;   /* non-dynamic requests */
    unicore_route_t *route;

    u_char          *SCRIPT_NAME;
    u_char          *PATH_INFO;
    u_char          *PATH_TRANSLATED;
    u_char          *QUERY_STRING;
    u_char          *GATEWAY_INTERFACE;
    u_int           REQUEST_METHOD;

    u_int           http_version;
    bool            cgi;
    bool            cgi_script_type;

} unicore_request_t;

#ifndef GET
    #define GET 0
#endif

#ifndef POST
    #define POST 1
#endif

#ifndef DELETE
   #define DELETE 1
#endif