

#include "../core/unicore_http_parse.hpp"

#include <iostream>

int main ()
{
    u_char *START_LINE_ACCEPTS[] = {
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
       (u_char *)"GET /$/$?///P/P?/PPP/?\x3F?//// HTTP/1.1\r\n",
       (u_char *)"\r\n\r\n\r\n\r\n GET /?    HTTP/1.1       \r\n",
       (u_char *)"DELETE /$/$$$$/$?PPPP/\x3F?/P/?//PPP????P HTTP/1.1\r\n",
       (u_char *)"GET /? HTTP/1.1\r\n",
       (u_char *)"HTTP/1.1 404 __visiblecharsfound:)_\r\n",
       (u_char *)"GET / HTTP/1.1 \n",
       (u_char *)"HTTP/1.1 326 \n",
    };

    u_char *START_LINE_REJECTS[] = {
        (u_char *)"GET / HTTP/1.2",
        (u_char *)"GET / HTTP/1.2\r\n",
        (u_char *)"  GET / HTTP/1.2\r\n\r\n",
        (u_char *)"POST / HTTP/1..1\r\n",
        (u_char *)"POST / HTTP/1 1\r\n",
        (u_char *)"POST /$segment/$/$ HTTP/1..1\r\n",
        (u_char *)"DELETE\r\n / HTTP/1.1\r\n",
        (u_char *)"get / http/1.1\r\n",
        (u_char *)"post / http/1.1\r\n",
        (u_char *)"delete / http/1.1\r\n",
        (u_char *)"HTTP/1.1 30 \r\n",
        (u_char *)"HTTP/1.1 14 \r\n",
        (u_char *)"HTTP/1.1 24 \r\n",
        (u_char *)"HTTP/1.1 040 \r\n",
        (u_char *)"HTTP/1.1 53 \r\n",
        (u_char *)"HTTP/1.1 47 \r\n",
        (u_char *)"HTTP/1.1    40 4    \r\n",
        (u_char *)"HTTP/1.1 404 \r\nHTTP/1.1 200\r\n",
        (u_char *)"HTTP/1.1 201 _\r\nHTTP/1.1 234 _\r\n",
        (u_char *)"HTTP/1.1 306\r\n\r\n",
        (u_char *)"HTTP/1.1 288  \r\n\r\n",
        (u_char *)"HTTP/1.1 103 \r",
    };


    // unicore_buf_t b = { start_line_test_str , start_line_test_str , start_line_test_str+24 };
    unicore_request_t r;
    r.state = 0;
    unicore_status_t s;

    std::cout << unicore_http_parse_start_line ( &r, &b, &s );

    return 0;

}