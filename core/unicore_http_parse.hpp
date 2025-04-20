

#pragma once
#include "unicore_buf.hpp"
#include "unicore_request.hpp"
#include "unicore_status.hpp"

int    
unicore_http_parse_start_line ( unicore_request_t *r , unicore_buf_t *b , unicore_status_t *s );