

#include "core/unicore_http_parse.hpp"

#include <iostream>

int main ()
{
    u_char  *start_line_test_str = (u_char *)"GET / HTTP/1.2";


    unicore_buf_t b = { start_line_test_str , start_line_test_str , start_line_test_str+24 };
    unicore_request_t r;
    r.state = 0;
    unicore_status_t s;

    std::cout << unicore_http_parse_start_line ( &r, &b, &s );

    return 0;

}