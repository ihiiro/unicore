

#include "core/unicore_http_parse.hpp"

#include <iostream>

int main ()
{
    u_char *START_LINES[] = {
       (u_char *)"\r\n    HTTP/1.1 200      \r\n",
       (u_char *)"HTTP/1.1 599 \n",
       (u_char *)"HTTP/1.1 300 \n",
       (u_char *)"HTTP/1.1 134 \n",
       (u_char *)"\r\n\r\n HTTP/1.1 400 \r\n",
       (u_char *)"POST /$ HTTP/1.1\r\n",
       (u_char *)"DELETE /somepage?// HTTP/1.1\r\n",
       (u_char *)"GET /segmentshit HTTP/1.1 \r\n",
       (u_char *)"GET /toilet/hole?dinner=free&firep=shit?/ HTTP/1.1\r\n",
       (u_char *)" GET /segment0/segment1/$subdelim/ HTTP/1.1\r\n",
       (u_char *)" GET / HTTP/1.1\r\n",
       (u_char *)"POST / HTTP/1.1\r\n",
       (u_char *)"DELETE / HTTP/1.1\r\n",
       (u_char *)"GET /$/$?///P/P?/PPP/???//// HTTP/1.1\r\n",
       (u_char *)"\r\n\r\n\r\n\r\n GET /?    HTTP/1.1       \r\n",
       (u_char *)"DELETE /$/$$$$/$?PPPP/??/P/?//PPP????P HTTP/1.1\r\n",
       (u_char *)"GET /? HTTP/1.1\r\n",
       (u_char *)"HTTP/1.1 404 __visiblecharsfound:)_\r\n",
       (u_char *)"GET / HTTP/1.1 \n",
       (u_char *)"HTTP/1.1 326 \n",
    }


    unicore_buf_t b = { start_line_test_str , start_line_test_str , start_line_test_str+24 };
    unicore_request_t r;
    r.state = 0;
    unicore_status_t s;

    std::cout << unicore_http_parse_start_line ( &r, &b, &s );

    return 0;

}