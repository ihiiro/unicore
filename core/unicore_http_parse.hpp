

#pragma once
#include "unicore_buf.hpp"
#include "unicore_request.hpp"
#include "unicore_config_parse.hpp"

#ifndef PYTHON
    #define PYTHON 0
#endif

#ifndef PHP
    #define PHP 1
#endif

int unicore_http_parse_request_line ( unicore_request_t *r , unicore_buf_t *b 
            , unicore_config_t& c );
int unicore_http_parse_field_lines ( unicore_request_t *r , unicore_buf_t *b );

int unicore_http_parse_chunked_body ( char *read_from , char *write_to , bool &chunked );