

#pragma once
#include "unicore_buf.hpp"
#include "unicore_request.hpp"
#include "unicore_config_parse.hpp"

#include <fstream>

#include <string>

#ifndef PYTHON
    #define PYTHON 0
#endif

#ifndef PHP
    #define PHP 1
#endif

typedef struct
{


    u_char                              *p;
    u_char                              *key;
    u_char                              *value;
    bucket                              *buckett;
    unicore_request_t                   *r;

    char                                boundary [ 70 ];
    char                                name [ 129 ];
    char                                filename [ 129 ];
    char                                content_type [ 129 ];


    size_t                              chunk_size;

    u_char                              ch;
    u_char                              primary_buf [ 512 ];
    u_char                              secondary_buf [ 512 ];
    u_char                              tertiary_buf [ 512 ];

    int                                 primary_i;
    int                                 secondary_i;
    int                                 tertiary_i;

    int                                 path_info_start;
    int                                 portion;
    int                                 state;

    int                                 hex_count;

    int                                 chunked_trailers_fsm_return;

    int                                 content_length;
    int                                 boundary_length;

    int                                 mp_i;

    bool                                chunked;

    std::ofstream                       *file;

    ht                                  *mimes;

    bucket                              *selected_mime_type;

} fsm_state_t;

int unicore_http_parse_request_line ( fsm_state_t& fsm_state , unicore_buf_t *b , unicore_config_t& c );

int unicore_http_parse_field_lines ( fsm_state_t& fsm_state , unicore_buf_t *b );

int unicore_http_parse_chunked_body ( fsm_state_t& fsm_state , unicore_buf_t *b );

int unicore_http_parse_multipart_body ( fsm_state_t& fsm_state , unicore_buf_t *b );

int unicore_http_parse_message_body ( fsm_state_t& fsm_state , unicore_buf_t *b );
