

#include "../core/unicore_http_parse.hpp"
#include "../core/unicore_config_parse.hpp"
#include <vector>

#include <cstring>

#include <iostream>

#include <fstream>

int main ()
{

    fsm_state_t fsm_state;
    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.r = new unicore_request_t;
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

    unicore_request_t r; // move the hash table initialization to START
    (void)r;
    fsm_state.r->headers = new ht;
    fsm_state.r->headers->buckets = new bucket [ M ];
    std::memset ( fsm_state.r->headers->buckets , 0 , M );
    unicore_buf_t b;

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

        // std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
        // fsm_state.r = new unicore_request_t;
        b.start = FIELD_LINES_ACCEPTS [ i ];
        b.pos = FIELD_LINES_ACCEPTS [ i ];
        b.end = b.start + std::strlen ( (const char *)FIELD_LINES_ACCEPTS [ i ] ) - 1;
        if ( unicore_http_parse_field_lines ( fsm_state, &b ) == 1 )
            std::cout << "\e[0;32mpass, ";
        else
            std::cout << "\e[0;31mfail, ";
        fsm_state.state = 0;

    }

    std::cout << "\e[0m\nREJECTS\n";
    for ( int i = 0; i < 4; i++ )
    {

        // std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
        // fsm_state.r = new unicore_request_t;       
        b.start = FIELD_LINES_REJECTS [ i ];
        b.pos = FIELD_LINES_REJECTS [ i ];
        b.end = b.start + std::strlen ( (const char *)FIELD_LINES_REJECTS [ i ] ) - 1;
        if ( i == 0 )
        {

            int validator = unicore_http_parse_field_lines ( fsm_state, &b );
            if ( validator != 1 and validator != 2 )
                std::cout << "\e[0;32mpass, ";
            else
                std::cout << "\e[0;31mfail, ";
            continue;

        }
        if ( unicore_http_parse_field_lines ( fsm_state, &b ) == 2 )
            std::cout << "\e[0;32mpass, ";
        else
            std::cout << "\e[0;31mfail, ";
        fsm_state.state = 0;

    }

    std::cout << "\n\t\t\t\t\e[1;37mHASHTABLE (FIELD LINES)\e[0m\n";
    if ( get ( fsm_state.r->headers , (u_char *)"" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( get ( fsm_state.r->headers , (u_char *)"f!#ld_" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( get ( fsm_state.r->headers , (u_char *)"transfer-encoding" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( fsm_state.r->headers , (u_char *)"!!####`|veryvalidfield&'" )->value,
    "value  VALUE") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( fsm_state.r->headers , (u_char *)"field" )->value,
    "Vvv") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( fsm_state.r->headers , (u_char *)"if-none-match" )->value,
    "SOME-VALID-VCHAR") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( fsm_state.r->headers , (u_char *)"someheader" )->value,
    "Valeur    \t\t   \t") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( fsm_state.r->headers , (u_char *)"another#one" )->value,
    "value      \t\t") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( fsm_state.r->headers , (u_char *)"t" )->value,
    "Vv v    \t\t") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( fsm_state.r->headers , (u_char *)"imf-fixdate" )->value,
    "V\t \t    vvvv") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( fsm_state.r->headers , (u_char *)"my-own-stupid-header" )->value,
    "STUPID-VALUE\t\t\tmorevalueandshit") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( fsm_state.r->headers , (u_char *)"headerheaderheaderheaderheader##--++" )->value,
    "V    vv   ") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( fsm_state.r->headers , (u_char *)"if-i-ever-were-to-lose-you" )->value,
    "I'd-surely-lose+my+self") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( get ( fsm_state.r->headers , (u_char *)"tttttt-tttttttt" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( get ( fsm_state.r->headers , (u_char *)"tttttttttttttttt" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( get ( fsm_state.r->headers , (u_char *)"tttttttttttttt" ) == NULL )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( !std::strcmp((char *)get ( fsm_state.r->headers , (u_char *)"ttttttttttttttttt" )->value,
    "V     vdksjeh3irhfidjfe  v\trefhdsjfdkfdjsfke383944+-'") )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    std::cout << "\n\t\t\t\t\e[1;37mCONFIGURATION\e[0m\n";
    std::cout << "ACCEPTS\n";
    char *passes[] = {
        (char *)"tests/pass0.conf",
        (char *)"tests/pass1.conf",
        (char *)"tests/pass2.conf",
        (char *)"tests/pass3.conf",
        (char *)"tests/pass4.conf",
        (char *)"tests/pass5.conf",
        (char *)"tests/pass6.conf",
        (char *)"tests/pass7.conf",
        (char *)"tests/pass8.conf"
    };

    std::vector < unicore_config_t > conf;
    for ( size_t i = 0 ; i < conf.size () ; i++ )
        std::memset ( &conf [ i ] , 0 , sizeof ( unicore_config_t ) );

    for ( int i = 0 ; i < 9 ; i++ )
    {

        std::ifstream stream ( passes [ i ] );
        if ( unicore_config_parse ( stream , conf ) == 1 )
            std::cout << "\e[0;32mpass, ";
        else
            std::cout << "\e[0;31mfail, ";

    }

    std::cout << "\n\t\t\t\t\e[1;37mCONFIGURATION DATA\e[0m\n";
    std::ifstream stream ( "tests/config_pass.conf" );

    
    std::vector < unicore_config_t > config;

	std::cout << unicore_config_parse ( stream , config ); //exit(1);
	if ( !strcmp ( "HOST" , config [ 0 ].host ) )
		std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
	if ( config [ 0 ].port == 8080 )
		std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( config [ 0 ].max_client_message_size == 10737418240 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
	// if ( !strcmp ( "server_name" , config [ 0 ].server_name ) )
	// 	std::cout << "\e[0;32mpass, ";
	// else
	// 	std::cout << "\e[0;31mfail, ";
	if ( get ( config [ 0 ].error_pages , (u_char *)"nonexistent" ) == NULL )
		std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
	if ( !strcmp ( (char *)get ( config [ 0 ].error_pages , (u_char *)"404" )->value , "404.html" ) )
		std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
	if ( !strcmp ( (char *)get ( config [ 0 ].error_pages , (u_char *)"434" )->value , "/hellbound_errors/434.html" ) )
		std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
	if ( get ( config [ 0 ].routes , (u_char *)"nonexistent" ) == NULL )
		std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
	unicore_route_t *route = (unicore_route_t *)get ( config [ 0 ].routes , (u_char *)"/route" )->value;
	if ( !strcmp ( route->root , "/root_dir" ) )
		std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( !strcmp ( route->file_if_directory_request , "file" ) )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->upload_path == NULL )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->ROUTE_GET == 1 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->ROUTE_POST == 0 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->ROUTE_DELETE == 1 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->ROUTE_UPLOADS == 0 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->CGI_GET == 1 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->CGI_POST == 1 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->CGI_PYTHON == 0 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->CGI_PHP == 0 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->directory_listing == 0 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    route = (unicore_route_t *)get ( config [ 0 ].routes , (u_char *)"/" )->value;
	if ( !strcmp ( route->root , "/" ) )
		std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( !strcmp ( route->file_if_directory_request , "FILE" ) )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( !strcmp ( "uploads/videos" , route->upload_path ) )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->ROUTE_GET == 0 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->ROUTE_POST == 0 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->ROUTE_DELETE == 0 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->ROUTE_UPLOADS == 1 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->CGI_GET == 1 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->CGI_POST == 1 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->CGI_PYTHON == 1 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->CGI_PHP == 1 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( route->directory_listing == 1 )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( get ( config [ 0 ].redirection_list , (u_char *)"shitdoesntexist" ) == NULL )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( !strcmp ( "/newurl/newshit" , (char *)get ( config [ 0 ].redirection_list , (u_char *)"/oldurl/smshit" )->value ) )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    if ( !strcmp ( "/oui" , (char *)get ( config [ 0 ].redirection_list , (u_char *)"/another" )->value ) )
    	std::cout << "\e[0;32mpass, ";
	else
		std::cout << "\e[0;31mfail, ";
    
    std::ifstream stream1 ( "tests/config_pass.conf" );

    unicore_config_parse ( stream1 , conf );

    std::cout << "\n\t\t\t\t\e[1;37mREQUEST-LINE\e[0m\n";
    std::cout << "ACCEPTS\n";
    for ( int i = 0; i < 14; i++ )
    {

        std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
        b.start = REQUEST_LINE_ACCEPTS [ i ];
        b.pos = REQUEST_LINE_ACCEPTS [ i ];
        b.end = b.start + std::strlen ( (const char *)REQUEST_LINE_ACCEPTS [ i ] ) - 1;
        if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 1 )
            std::cout << "\e[0;32mpass, ";
        else
            std::cout << "\e[0;31mfail, ";

    }

    std::cout << "\e[0m\nREJECTS\n";
    for ( int i = 0; i < 10; i++ )
    {

        std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
        b.start = REQUEST_LINE_REJECTS [ i ];
        b.pos = REQUEST_LINE_REJECTS [ i ];
        b.end = b.start + std::strlen ( (const char *)REQUEST_LINE_REJECTS [ i ] ) - 1;
        if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) != 1 )
            std::cout << "\e[0;32mpass, ";
        else
            std::cout << "\e[0;31mfail, ";

    }

    std::cout << "\n\t\t\t\t\e[1;37mCHUNKED-MESSAGE\e[0m\n";

    u_char *CHUNKED_MESSAGE_ACCEPTS[] = {
       ( u_char * )"0\r\n\r\n",
       ( u_char * )"000000\r\n\r\n",
       ( u_char * )"000\n\n",
       ( u_char * )"0\n\r\n",
       ( u_char * )"0000   \t;  tchar \t=TCHAR\n\n",
       ( u_char * )"0000;tchar\t=tcharTCHAR\r\n\r\n",
       ( u_char * )"0000;ttttttttttt\r\n\r\n",
       ( u_char * )"0;ttt\t=tchar\r\n\r\n",
       ( u_char * )"0000a\nis10octets\r\n",
       ( u_char * )"0000c\r\nis11octets_\r\n",
       ( u_char * )"00000c;pp\t=tit\r\nndis12octets\r\n",
       ( u_char * )"00000C;pp\t=tit\r\nndis12octets\n",
       ( u_char * )"00000C;pp\t=tit\nndis12octets\n",
       ( u_char * )"A \t\t\t\t ;             \t\t\tthis\t=that\n0123456789\n",
       ( u_char * )"A \t\t\t\t ;             this\t =\tthat\r\n0123456789\n",
       ( u_char * )"A \t\t\t\t ;             this\r\n0123456789\n",
    };

    std::cout << "ACCEPTS\n";
    for ( int i = 0; i < 16; i++ )
    {

        int validator;
        if ( i < 8 )
            validator = 1;
        else
            validator = 2;
        std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
        fsm_state.file = new std::ofstream;
        b.start = CHUNKED_MESSAGE_ACCEPTS [ i ];
        b.pos = b.start;
        b.end = b.start + std::strlen ( ( char * )CHUNKED_MESSAGE_ACCEPTS [ i ]) - 1;
        if (  unicore_http_parse_chunked_body ( fsm_state , &b ) == validator )
            std::cout << "\e[0;32mpass, ";
        else
            std::cout << "\e[0;31mfail, ";

    }

    std::cout << "\n\t\t\t\t\e[1;37mFREEZABLE-CHUNKED-FSM\e[0m\n";
    // "A \t\t\t\t ;             this\r\n0123456789\n"
    u_char *chunked0 = ( u_char * )"A \t\t\t\t ;";
    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.file = new std::ofstream;
    b.start = chunked0;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )chunked0 ) - 1;
    if ( unicore_http_parse_chunked_body ( fsm_state , &b ) == 2 and fsm_state.state == 3 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *chunked1 = ( u_char * )"           ";
    b.start = chunked1;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )chunked1 ) - 1;
    if ( unicore_http_parse_chunked_body ( fsm_state , &b ) == 2 and fsm_state.state == 4 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *chunked2 = ( u_char * )"  ";
    b.start = chunked2;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )chunked2 ) - 1;
    if ( unicore_http_parse_chunked_body ( fsm_state , &b ) == 2 and fsm_state.state == 4 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *chunked3 = ( u_char * )"this\r\n01234";
    b.start = chunked3;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )chunked3 ) - 1;
    if ( unicore_http_parse_chunked_body ( fsm_state , &b ) == 2 and fsm_state.state == 12 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *chunked4 = ( u_char * )"56789";
    b.start = chunked4;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )chunked4 ) - 1;
    if ( unicore_http_parse_chunked_body ( fsm_state , &b ) == 2 and fsm_state.state == 12 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *chunked5 = ( u_char * )"\n";
    b.start = chunked5;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )chunked5 ) - 1;
    if ( unicore_http_parse_chunked_body ( fsm_state , &b ) == 2 and fsm_state.state == 14 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    std::cout << "\n\t\t\t\t\e[1;37mFREEZABLE-REQUEST-LINE-FSM\e[0m\n";
    // "\r\n\r\n\r\n\r\n GET /?    HTTP/1.1       \r\n"
    u_char *rl0 = ( u_char * )"\r\n\r\n\r\n\r\n ";
    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    b.start = rl0;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )rl0 ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 2 and fsm_state.state == 3 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *rl1 = ( u_char * )"GET /?    HTTP/1";
    b.start = rl1;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )rl1 ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 2 and fsm_state.state == 31 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *rl2 = ( u_char * )".1       \r";
    b.start = rl2;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )rl2 ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 2 and fsm_state.state == 4 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *rl3 = ( u_char * )"\n";
    b.start = rl3;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )rl3 ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 1 and fsm_state.state == 0 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    std::cout << "\n\t\t\t\t\e[1;37mFREEZABLE-FIELD-LINES-FSM\e[0m\n";
    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.r = new unicore_request_t;
    fsm_state.r->headers = new ht;
    fsm_state.r->headers->buckets = new bucket [ M ];
    std::memset ( fsm_state.r->headers->buckets , 0 , M );
    // "!!####`|veryvalidfield&'B:value  VALUE\r\nFIELD: Vvv\r\n\r\n"
    u_char *fl0 = ( u_char * )"!!####";
    b.start = fl0;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )fl0 ) - 1;
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 2 and fsm_state.state == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *fl1 = ( u_char * )"`|veryvalidfield&'B:";
    b.start = fl1;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )fl1 ) - 1;
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 2 and fsm_state.state == 2 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *fl2 = ( u_char * )"value  VALUE\r\n";
    b.start = fl2;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )fl2 ) - 1;
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 2 and fsm_state.state == 11 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *fl3 = ( u_char * )"FIELD";
    b.start = fl3;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )fl3 ) - 1;
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 2 and fsm_state.state == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *fl4 = ( u_char * )":";
    b.start = fl4;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )fl4 ) - 1;
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 2 and fsm_state.state == 2 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *fl5 = ( u_char * )" ";
    b.start = fl5;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )fl5 ) - 1;
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 2 and fsm_state.state == 3 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    u_char *fl6 = ( u_char * )"Vvv\r\n\r\n";
    b.start = fl6;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )fl6 ) - 1;
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 1 and fsm_state.state == 0 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    std::cout << "\n\t\t\t\t\e[1;37mMULTIPART-FSM\e[0m\n";
    u_char *MULTIPART_ACCEPTS[] = {

       ( u_char * )"--boundary  \t\t \r\n\r\ndata\r\n--boundary--",
       ( u_char * )"--boundary\r\n\r\ndata\r\n--boundary--",
       ( u_char * )"--boundary\r\n\r\ndata\r\n--boundary--",
       ( u_char * )"--boundary\r\n\r\ndata\r\n--boundary\r\n\r\ndata\r\n--boundary--",
       ( u_char * )"--boundary\r\nContent-Disposition:form-data;name=tchar;filename=tchar\r\n\r\ndata\r\n--boundary--",
       ( u_char * )"--boundary\r\nContent-Disposition:\t\t\tform-data  \t\t\t\t ; \t\t\t\tname=tchar\t\t\t\t\t;\t\t\t\t\t\tfilename=tchar\t\t\t\r\n\r\ndata\r\n--boundary--",
       ( u_char * )"--boundary\r\nContent-Disposition: form-data; name=tchar;  filename=tchar\r\nContent-Type: image/jpg\r\n\r\ndata\r\n--boundary--",
       ( u_char * )"--boundary\r\nContent-Disposition: form-data; name=tchar; filename=tchar\r\nContent-Type: image/png\r\n\r\ndata\r\n--boundary\r\nContent-Disposition: form-data; name=tchar; filename=tchar\r\nContent-Type: image/jpg\r\n\r\ndata\r\n--boundary--",
       ( u_char * )"--boundary\r\nContent-Disposition: form-data; name=\"quoted_tchar\"; filename=\"quoted_tchar\"\r\nContent-Type: text/plain\r\n\r\ndata\r\n--boundary\r\nContent-Disposition: form-data; name=tchar; filename=tchar\r\nContent-Type: image/png\r\n\r\ndata\r\n--boundary--",
       ( u_char * )"--boundary\r\nContent-Disposition: form-data; name=\"filename\"; filename=\"download.jpg\"\r\nContent-Type: image/jpeg\r\n\r\ndata\r\n--boundary\r\nContent-Disposition: form-data; name=\"\"; filename=\"238971-400596355.jpg\"\r\nContent-Type: image/jpeg\r\n\r\ndata\r\n--boundary--",
       ( u_char * )"--boundary\r\nContent-Disposition: form-data; name=\"filename\"; filename=\"download.jpg\"\r\nContent-Type: image/jpeg\r\n\r\ndata\r\n--boundary\r\nContent-Disposition: form-data; name=\"\"; filename=\"\"\r\nContent-Type: image/jpeg\r\n\r\ndata\r\n--boundary--"

    };

    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.r = new unicore_request_t;
    fsm_state.r->route = new unicore_route_t;
    fsm_state.r->route->upload_path = ( char * )"PAUL";
    fsm_state.r->route->root = ( char * )"/";
    fsm_state.file = new std::ofstream;
    fsm_state.boundary [ 0 ] = 'b';
    fsm_state.boundary [ 1 ] = 'o';
    fsm_state.boundary [ 2 ] = 'u';
    fsm_state.boundary [ 3 ] = 'n';
    fsm_state.boundary [ 4 ] = 'd';
    fsm_state.boundary [ 5 ] = 'a';
    fsm_state.boundary [ 6 ] = 'r';
    fsm_state.boundary [ 7 ] = 'y';
    fsm_state.boundary_length = 8;

    for ( int i = 0 ; i < 11 ; i++ )
    {

        b.start = MULTIPART_ACCEPTS [ i ];
        fsm_state.content_length = std::strlen ( ( char * )b.start );
        b.pos = b.start;
        b.end = b.start + std::strlen ( ( char * )b.start ) - 1;
        if ( unicore_http_parse_multipart_body ( fsm_state , &b ) == 1 )
            std::cout << "\e[0;32mpass, ";
        else
            std::cout << "\e[0;31mfail, ";
        // std::cerr << "\n";

    }

    std::cout << "\n\t\t\t\t\e[1;37mGENERAL PARSER INTEGRATION TESTS\e[0m\n";

    u_char *request_str = ( u_char * )"GET /?df=df HTTP/1.1\r\nHost: localhost:8000\r\nContent-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW" \
                        "\r\nContent-Length: 229\r\n\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"\"; filename=\"\"" \
                        "\r\n\r\ndata\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--";

    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.file = new std::ofstream;
    b.start = request_str;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )request_str ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.file = new std::ofstream;
    u_char *request_str_0 = ( u_char * )"GET /?df=df HTTP/1.1\r\nHost: localhost:8000\r\nContent-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW" \
            "\r\nContent-Length: 229\r\n\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"filename\"; filename=\"dinr";

    u_char *request_str_400 = ( u_char * )"eb.txt\"\r\nContent-Type: image/jpeg\r\n\r\ndata\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--";
    b.start = request_str_0;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )request_str_0 ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 2 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    b.start = request_str_400;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )request_str_400 ) - 1;
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";


    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.file = new std::ofstream;
    u_char *request_str_q2prago = ( u_char * )"GET / HTTP/1.1\r\nHost: localhost:8000\r\nContent-Type:image/gif\r\ncontent-length:  \t4" \
                    "\r\n\r\ndataff";
    b.start = request_str_q2prago;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )request_str_q2prago ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";


    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.file = new std::ofstream;
    u_char *request_str_terron = ( u_char * )"GET / HTTP/1.1\r\nHost: localhost:8000\r\nContent-Type:multipart/form-data;\r\ncontent-length:  \t4" \
                     "\r\n\r\ndat";

    u_char *request_str_south = ( u_char * )"anoff";

    b.start = request_str_terron;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )request_str_terron ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 2 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    b.start = request_str_south;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )request_str_south ) - 1;
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";


    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.file = new std::ofstream;
    u_char *venus = ( u_char * )"GET / HTTP/1.1\r\nHost: localhost:8000\r\ntransfer-encoding:chunked\r\n\r\n4\r\ndata\r\nA\r\ndata\r\ndata\r\n0\r\n\r\n";

    b.start = venus;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )venus ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.file = new std::ofstream;
    u_char *andromeda = ( u_char * )"GET / HTTP/1.1\r\nHost: localhost:8000\r\ncontent-type:      \t\t\t audio/aac\r\ntransfer-encoding:chunked\r\n\r\n4\r\ndata\r";

    u_char *karl_marx = ( u_char * )"\nA\r\ndata\r\ndata\r\n0\r\n\r\n";

    b.start = andromeda;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )andromeda ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 2 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    b.start = karl_marx;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )karl_marx ) - 1;
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";


    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.file = new std::ofstream;

    u_char *ajax_niner = ( u_char * )"GET /python.py HTTP/1.1\r\nHost: localhost:8000\r\nContent-Type:multipart/form-data;\r\ncontent-length:4\r\n\r\ndata";

    b.start = ajax_niner;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )ajax_niner ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    if ( fsm_state.r->route->message_body == "data" )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    std::memset ( &fsm_state , 0 , sizeof ( fsm_state_t ) );
    fsm_state.file = new std::ofstream;

    u_char *paul_alan = ( u_char * )"GET /route/python.py HTTP/1.1\r\nHost: localhost:8000\r\nContent-Type:multipart/form-data;\r\ncontent-length:4\r\n\r\nda";

    u_char *pat_bateman = ( u_char * )"ta";

    b.start = paul_alan;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )paul_alan ) - 1;
    if ( unicore_http_parse_request_line ( fsm_state , &b , conf [ 0 ] ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_field_lines ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 2 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    b.start = pat_bateman;
    b.pos = b.start;
    b.end = b.start + std::strlen ( ( char * )pat_bateman ) - 1;
    if ( unicore_http_parse_message_body ( fsm_state , &b ) == 1 )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    if ( fsm_state.r->route->message_body == "data" )
        std::cout << "\e[0;32mpass, ";
    else
        std::cout << "\e[0;31mfail, ";

    


    // std::cout << "\n";

    // std::cout << fsm_state.state;
    // std::cout << fsm_state.p;
    

    // u_char *request = (u_char *)"GET /route/.py/extrapath/shithtml/?name=yes HTTP/1.1\r\n";
    // // u_char *request = (u_char *)"GET /servlet?;jsessionid=1234&param=value/ HTTP/1.1 \r\n";
    // unicore_buf_t bb = { request , request , request + 54 };

    // std::cout << unicore_http_parse_request_line ( &r , &bb , conf [ 0 ] );

    // while (true);
    return 0;

}