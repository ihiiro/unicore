

#pragma once
#include <fstream>
#include "unicore_request.hpp"

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

typedef struct
{

    /* socket */

    char                *host;
    char                *server_name;

    ht                  *error_pages;        /* hashed by status code (bucket) */
    ht                  *routes;             /* hashed by route */
    ht                  *redirection_list;   /* hashed by old url (bucket) */  

    size_t              max_client_message_size;
    size_t              port;


} unicore_config_t;

int unicore_config_parse ( std::ifstream &s , unicore_config_t *c  );