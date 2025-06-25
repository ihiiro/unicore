

#pragma once
#include "unicore_buf.hpp"
#include "unicore_request.hpp"
#include "unicore_config_parse.hpp"

#include <string>

#ifndef PYTHON
    #define PYTHON 0
#endif

#ifndef PHP
    #define PHP 1
#endif

typedef struct
{

    u_char              *p;
    u_char              *key;
    u_char              *value;
    bucket              *bucket;
    unicore_request_t   *r;

    u_char              ch;
    u_char              primary_buf [ 512 ];
    u_char              secondary_buf [ 512 ];
    u_char              tertiary_buf [ 512 ];

    int                 primary_i;
    int                 secondary_i;
    int                 tertiary_i;

    int                 path_info_start;
    int                 portion;
    int                 state;

} fsm_state_t;

int unicore_http_parse_request_line ( fsm_state_t& fsm_state , unicore_buf_t *b 
            , unicore_config_t& c );

int unicore_http_parse_field_lines ( fsm_state_t& fsm_state , unicore_buf_t *b );

int unicore_http_parse_chunked_body ( unicore_request_t *r , char *read_from , std::string& write_to , bool &chunked );