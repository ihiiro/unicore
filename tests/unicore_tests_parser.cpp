

#include "../core/unicore_http_parse.hpp"
#include "../core/unicore_config_parse.hpp"

#include <cstring>

#include <iostream>

#include <fstream>

int main ()
{

    u_char *REQUEST_LINE_ACCEPTS[] = {
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
       (u_char *)"GET / HTTP/1.1 \n",
       (u_char *)"GET /index.html HTTP/1.1\r\n",
    };

    u_char *REQUEST_LINE_REJECTS[] = {
        (u_char *)"GET / HTTP/1.2",
        (u_char *)"GET / HTTP/1.2\r\n",
        (u_char *)"  GET / HTTP/1.2\r\n\r\n",
        (u_char *)"POST / HTTP/1..1\r\n",
        (u_char *)"POST / HTTP/1 1\r\n",
        (u_char *)"POST /$segment/$/$ HTTP/1..1\r\n",
        (u_char *)"DELETE\r\n / HTTP/1.1\r\n",
        (u_char *)"get / http/1.1\r\n",
        (u_char *)"post / http/1.1\r\n",
        (u_char *)"delete / http/1.1\r\n"
    };

    unicore_request_t r;
    r.headers = new ht;
    r.headers->buckets = new bucket [ M ];
    std::memset ( r.headers->buckets , 0 , M );
    unicore_status_t s;
    unicore_buf_t b;
    std::cout << "\t\t\t\t\e[1;37mREQUEST-LINE\e[0m\n";
    std::cout << "ACCEPTS\n";
    for ( int i = 0; i < 14; i++ )
    {

        b.start = REQUEST_LINE_ACCEPTS [ i ];
        b.pos = REQUEST_LINE_ACCEPTS [ i ];
        b.end = b.start + std::strlen ( (const char *)REQUEST_LINE_ACCEPTS [ i ] );
        if ( unicore_http_parse_request_line ( &r, &b, &s ) == 1 )
            std::cout << "\e[0;32mpass, ";
        else
            std::cout << "\e[0;31mfail, ";

    }

    std::cout << "\e[0m\nREJECTS\n";
    for ( int i = 0; i < 10; i++ )
    {

        b.start = REQUEST_LINE_REJECTS [ i ];
        b.pos = REQUEST_LINE_REJECTS [ i ];
        b.end = b.start + std::strlen ( (const char *)REQUEST_LINE_REJECTS [ i ] );
        if ( unicore_http_parse_request_line ( &r, &b, &s ) == -1 )
            std::cout << "\e[0;32mpass, ";
        else
            std::cout << "\e[0;31mfail, ";

    }

    u_char *FIELD_LINES_ACCEPTS[] = {
        (u_char *)"\n",
        (u_char *)"F!#Ld_:  \t\r\n\n",
        (u_char *)"Transfer-Encoding:     \r\n\r\n",
        (u_char *)"!!####`|veryvalidfield&':value  VALUE\r\nFIELD: Vvv\r\n\r\n",
        (u_char *)"If-None-Match:SOME-VALID-VCHAR\r\n\r\n",
        (u_char *)"If-None-Match:    \t\t  SOME-VALID-VCHAR\r\nIf-None-Match:        SOME-VALID-VCHAR\r\n\r\n",
        (u_char *)"SOMEHEADER: Valeur    \t\t   \t\r\nANOTHER#ONE:       value      \t\t\r\n\r\n",
        (u_char *)"T: Vv v    \t\t\r\n\r\n",
        (u_char *)"T: \t \t Vv\t\tvv    \t\t\t\t\t\t\t\r\n\r\n",
        (u_char *)"T:\t\tVv \t \tv\t\t\t\r\n\r\n",
        (u_char *)"IMF-fixdate:\t\tV\t \t    vvvv\r\n\r\n",
        (u_char *)"MY-OWN-STUPID-HEADER:STUPID-VALUE\t\t\tmorevalueandshit\r\n\r\n",
        (u_char *)"HEADERHEADERHEADERheaderHEADER##--++:\tV    vv   \r\n\r\n",
        (u_char *)"If-I-ever-were-to-lose-you:   I'd-surely-lose+my+self\r\n\r\n",
        (u_char *)"TTTTTT-tTTTTTTT: \t \t \t \t \r\n\r\n",
        (u_char *)"TTTTTTTTTTTTTTTT:                           \r\n\r\n",
        (u_char *)"TTTTTTTTTTTTTT:\r\n\r\n",
        (u_char *)"TTTTTTTTTTTTTTTTT:V     vdksjeh3irhfidjfe  v\trefhdsjfdkfdjsfke383944+-'\r\n\r\n",
        (u_char *)"T:V    \r\n\r\n"
    };
    u_char *FIELD_LINES_REJECTS[] = {
        (u_char *)"SOMEFIELDNAME\t : somevalueafterwhitespace\r\n\r\n",
        (u_char *)"TTTTTTTTTT:Vvv\t vv\r\n",
        (u_char *)"T:   Vvv\t\t vv",
        (u_char *)"T: V\r\nT:\tV\r\n",
    /*
will succeed because one CRLF terminates the field
   and the other CRLF is the mandatory CRLF before message-body
    */
        // (u_char *)"T:\t V\r\n\r\nT:   V\r\n\r\n"
    };

    std::cout << "\n\t\t\t\t\e[1;37mFIELD-LINES\e[0m\n";
    std::cout << "ACCEPTS\n";
    for ( int i = 0; i < 19; i++ )
    {

        b.start = FIELD_LINES_ACCEPTS [ i ];
        b.pos = FIELD_LINES_ACCEPTS [ i ];
        b.end = b.start + std::strlen ( (const char *)FIELD_LINES_ACCEPTS [ i ] );
        if ( unicore_http_parse_field_lines ( &r, &b, &s ) == 1 )
            std::cout << "\e[0;32mpass, ";
        else
            std::cout << "\e[0;31mfail, ";

    }

    std::cout << "\e[0m\nREJECTS\n";
    for ( int i = 0; i < 4; i++ )
    {

        b.start = FIELD_LINES_REJECTS [ i ];
        b.pos = FIELD_LINES_REJECTS [ i ];
        b.end = b.start + std::strlen ( (const char *)FIELD_LINES_REJECTS [ i ] );
        if ( unicore_http_parse_field_lines ( &r, &b, &s ) == -1 )
            std::cout << "\e[0;32mpass, ";
        else
            std::cout << "\e[0;31mfail, ";

    }

    std::cout << "\n\t\t\t\t\e[1;37mHASH TABLE\e[0m\n";
    if ( get ( r.headers , (u_char *)"" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( get ( r.headers , (u_char *)"f!#ld_" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( get ( r.headers , (u_char *)"transfer-encoding" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( r.headers , (u_char *)"!!####`|veryvalidfield&'" )->value,
    "value  VALUE") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( r.headers , (u_char *)"field" )->value,
    "Vvv") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( r.headers , (u_char *)"if-none-match" )->value,
    "SOME-VALID-VCHAR") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( r.headers , (u_char *)"someheader" )->value,
    "Valeur    \t\t   \t") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( r.headers , (u_char *)"another#one" )->value,
    "value      \t\t") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( r.headers , (u_char *)"t" )->value,
    "Vv v    \t\t") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( r.headers , (u_char *)"imf-fixdate" )->value,
    "V\t \t    vvvv") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( r.headers , (u_char *)"my-own-stupid-header" )->value,
    "STUPID-VALUE\t\t\tmorevalueandshit") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( r.headers , (u_char *)"headerheaderheaderheaderheader##--++" )->value,
    "V    vv   ") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( r.headers , (u_char *)"if-i-ever-were-to-lose-you" )->value,
    "I'd-surely-lose+my+self") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( get ( r.headers , (u_char *)"tttttt-tttttttt" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( get ( r.headers , (u_char *)"tttttttttttttttt" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( get ( r.headers , (u_char *)"tttttttttttttt" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( r.headers , (u_char *)"ttttttttttttttttt" )->value,
    "V     vdksjeh3irhfidjfe  v\trefhdsjfdkfdjsfke383944+-'") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    std::cout << "\n\t\t\t\t\e[1;37mCONFIGURATION\e[0m\n";
    std::ifstream pass0 ( "tests/pass0.conf" );


    unicore_config_t conf;

    std::cout << unicore_config_parse ( pass0 , &conf );


    return 0;

}