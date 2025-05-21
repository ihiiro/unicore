

#pragma once
#include <fstream>
#include "unicore_request.hpp"

typedef std::ifstream::traits_type traits_type;

typedef struct
{

    /* socket */

    char                *host;
    char                *server_name;

    ht                  *error_pages;        /* hashed by status code */
    ht                  *routes;             /* hashed by route */
    ht                  *redirection_list;   /* hashed by old url */  

    size_t              max_client_message_size;
    size_t              port;


} unicore_config_t;

int unicore_config_parse ( std::ifstream &s , unicore_config_t *c  );