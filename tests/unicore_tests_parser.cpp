

#include "../core/unicore_http_parse.hpp"
#include "../core/unicore_config_parse.hpp"
#include <vector>

#include <cstring>

#include <iostream>

#include <fstream>

int main ()
{

//     u_char *REQUEST_LINE_ACCEPTS[] = {
//        (u_char *)"POST /$ HTTP/1.1\r\n",
//        (u_char *)"DELETE /somepage?// HTTP/1.1\r\n",
//        (u_char *)"GET /segmentshit HTTP/1.1 \r\n",
//        (u_char *)"GET /toilet/hole?dinner=free&firep=shit?/ HTTP/1.1\r\n",
//        (u_char *)" GET /segment0/segment1/$subdelim/ HTTP/1.1\r\n",
//        (u_char *)" GET / HTTP/1.1\r\n",
//        (u_char *)"POST / HTTP/1.1\r\n",
//        (u_char *)"DELETE / HTTP/1.1\r\n",
//        (u_char *)"GET /$/$?///P/P?/PPP/?\x3F?//// HTTP/1.1\r\n",
//        (u_char *)"\r\n\r\n\r\n\r\n GET /?    HTTP/1.1       \r\n",
//        (u_char *)"DELETE /$/$$$$/$?PPPP/\x3F?/P/?//PPP????P HTTP/1.1\r\n",
//        (u_char *)"GET /? HTTP/1.1\r\n",
//        (u_char *)"GET / HTTP/1.1 \n",
//        (u_char *)"GET /index.html HTTP/1.1\r\n",
//     };

//     u_char *REQUEST_LINE_REJECTS[] = {
//         (u_char *)"GET / HTTP/1.2",
//         (u_char *)"GET / HTTP/1.2\r\n",
//         (u_char *)"  GET / HTTP/1.2\r\n\r\n",
//         (u_char *)"POST / HTTP/1..1\r\n",
//         (u_char *)"POST / HTTP/1 1\r\n",
//         (u_char *)"POST /$segment/$/$ HTTP/1..1\r\n",
//         (u_char *)"DELETE\r\n / HTTP/1.1\r\n",
//         (u_char *)"get / http/1.1\r\n",
//         (u_char *)"post / http/1.1\r\n",
//         (u_char *)"delete / http/1.1\r\n"
//     };

    unicore_request_t r;
//     r.headers = new ht;
//     r.headers->buckets = new bucket [ M ];
//     std::memset ( r.headers->buckets , 0 , M );
//     unicore_buf_t b;

//     u_char *FIELD_LINES_ACCEPTS[] = {
//         (u_char *)"\n",
//         (u_char *)"F!#Ld_:  \t\r\n\n",
//         (u_char *)"Transfer-Encoding:     \r\n\r\n",
//         (u_char *)"!!####`|veryvalidfield&':value  VALUE\r\nFIELD: Vvv\r\n\r\n",
//         (u_char *)"If-None-Match:SOME-VALID-VCHAR\r\n\r\n",
//         (u_char *)"If-None-Match:    \t\t  SOME-VALID-VCHAR\r\nIf-None-Match:        SOME-VALID-VCHAR\r\n\r\n",
//         (u_char *)"SOMEHEADER: Valeur    \t\t   \t\r\nANOTHER#ONE:       value      \t\t\r\n\r\n",
//         (u_char *)"T: Vv v    \t\t\r\n\r\n",
//         (u_char *)"T: \t \t Vv\t\tvv    \t\t\t\t\t\t\t\r\n\r\n",
//         (u_char *)"T:\t\tVv \t \tv\t\t\t\r\n\r\n",
//         (u_char *)"IMF-fixdate:\t\tV\t \t    vvvv\r\n\r\n",
//         (u_char *)"MY-OWN-STUPID-HEADER:STUPID-VALUE\t\t\tmorevalueandshit\r\n\r\n",
//         (u_char *)"HEADERHEADERHEADERheaderHEADER##--++:\tV    vv   \r\n\r\n",
//         (u_char *)"If-I-ever-were-to-lose-you:   I'd-surely-lose+my+self\r\n\r\n",
//         (u_char *)"TTTTTT-tTTTTTTT: \t \t \t \t \r\n\r\n",
//         (u_char *)"TTTTTTTTTTTTTTTT:                           \r\n\r\n",
//         (u_char *)"TTTTTTTTTTTTTT:\r\n\r\n",
//         (u_char *)"TTTTTTTTTTTTTTTTT:V     vdksjeh3irhfidjfe  v\trefhdsjfdkfdjsfke383944+-'\r\n\r\n",
//         (u_char *)"T:V    \r\n\r\n"
//     };
//     u_char *FIELD_LINES_REJECTS[] = {
//         (u_char *)"SOMEFIELDNAME\t : somevalueafterwhitespace\r\n\r\n",
//         (u_char *)"TTTTTTTTTT:Vvv\t vv\r\n",
//         (u_char *)"T:   Vvv\t\t vv",
//         (u_char *)"T: V\r\nT:\tV\r\n",
//     /*
// will succeed because one CRLF terminates the field
//    and the other CRLF is the mandatory CRLF before message-body
//     */
//         // (u_char *)"T:\t V\r\n\r\nT:   V\r\n\r\n"
//     };

//     std::cout << "\n\t\t\t\t\e[1;37mFIELD-LINES\e[0m\n";
//     std::cout << "ACCEPTS\n";
//     for ( int i = 0; i < 19; i++ )
//     {

//         b.start = FIELD_LINES_ACCEPTS [ i ];
//         b.pos = FIELD_LINES_ACCEPTS [ i ];
//         b.end = b.start + std::strlen ( (const char *)FIELD_LINES_ACCEPTS [ i ] );
//         if ( unicore_http_parse_field_lines ( &r, &b ) == 1 )
//             std::cout << "\e[0;32mpass, ";
//         else
//             std::cout << "\e[0;31mfail, ";

//     }

//     std::cout << "\e[0m\nREJECTS\n";
//     for ( int i = 0; i < 4; i++ )
//     {

//         b.start = FIELD_LINES_REJECTS [ i ];
//         b.pos = FIELD_LINES_REJECTS [ i ];
//         b.end = b.start + std::strlen ( (const char *)FIELD_LINES_REJECTS [ i ] );
//         if ( unicore_http_parse_field_lines ( &r, &b ) == -1 )
//             std::cout << "\e[0;32mpass, ";
//         else
//             std::cout << "\e[0;31mfail, ";

//     }

//     std::cout << "\n\t\t\t\t\e[1;37mHASHTABLE (FIELD LINES)\e[0m\n";
//     if ( get ( r.headers , (u_char *)"" ) == NULL )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( get ( r.headers , (u_char *)"f!#ld_" ) == NULL )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( get ( r.headers , (u_char *)"transfer-encoding" ) == NULL )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( !std::strcmp((char *)get ( r.headers , (u_char *)"!!####`|veryvalidfield&'" )->value,
//     "value  VALUE") )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( !std::strcmp((char *)get ( r.headers , (u_char *)"field" )->value,
//     "Vvv") )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( !std::strcmp((char *)get ( r.headers , (u_char *)"if-none-match" )->value,
//     "SOME-VALID-VCHAR") )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( !std::strcmp((char *)get ( r.headers , (u_char *)"someheader" )->value,
//     "Valeur    \t\t   \t") )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( !std::strcmp((char *)get ( r.headers , (u_char *)"another#one" )->value,
//     "value      \t\t") )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( !std::strcmp((char *)get ( r.headers , (u_char *)"t" )->value,
//     "Vv v    \t\t") )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( !std::strcmp((char *)get ( r.headers , (u_char *)"imf-fixdate" )->value,
//     "V\t \t    vvvv") )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( !std::strcmp((char *)get ( r.headers , (u_char *)"my-own-stupid-header" )->value,
//     "STUPID-VALUE\t\t\tmorevalueandshit") )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( !std::strcmp((char *)get ( r.headers , (u_char *)"headerheaderheaderheaderheader##--++" )->value,
//     "V    vv   ") )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( !std::strcmp((char *)get ( r.headers , (u_char *)"if-i-ever-were-to-lose-you" )->value,
//     "I'd-surely-lose+my+self") )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( get ( r.headers , (u_char *)"tttttt-tttttttt" ) == NULL )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( get ( r.headers , (u_char *)"tttttttttttttttt" ) == NULL )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( get ( r.headers , (u_char *)"tttttttttttttt" ) == NULL )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";
//     if ( !std::strcmp((char *)get ( r.headers , (u_char *)"ttttttttttttttttt" )->value,
//     "V     vdksjeh3irhfidjfe  v\trefhdsjfdkfdjsfke383944+-'") )
//         std::cout << "\e[0;32mpass, ";
//     else
//         std::cout << "\e[0;31mfail, ";

//     std::cout << "\n\t\t\t\t\e[1;37mCONFIGURATION\e[0m\n";
//     char *passes[] = {
//         (char *)"tests/pass0.conf",
//         (char *)"tests/pass1.conf",
//         (char *)"tests/pass2.conf",
//         (char *)"tests/pass3.conf",
//         (char *)"tests/pass4.conf",
//         (char *)"tests/pass5.conf",
//         (char *)"tests/pass6.conf",
//         (char *)"tests/pass7.conf",
//         (char *)"tests/pass8.conf"
//     };

    std::vector < unicore_config_t > conf;
//     for ( size_t i = 0 ; i < conf.size () ; i++ )
//         std::memset ( &conf [ i ] , 0 , sizeof ( unicore_config_t ) );

//     for ( int i = 0 ; i < 9 ; i++ )
//     {

//         std::ifstream stream ( passes [ i ] );
//         if ( unicore_config_parse ( stream , conf ) == 1 )
//             std::cout << "\e[0;32mpass, ";
//         else
//             std::cout << "\e[0;31mfail, ";

//     }

//     std::cout << "\n\t\t\t\t\e[1;37mCONFIGURATION DATA\e[0m\n";
//     std::ifstream stream ( "tests/config_pass.conf" );

    
//     std::vector < unicore_config_t > config;

// 	unicore_config_parse ( stream , config );
// 	if ( !strcmp ( "HOST" , config [ 0 ].host ) )
// 		std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
// 	if ( config [ 0 ].port == 8080 )
// 		std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( config [ 0 ].max_client_message_size == 10737418240 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
// 	if ( !strcmp ( "server_name" , config [ 0 ].server_name ) )
// 		std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
// 	if ( get ( config [ 0 ].error_pages , (u_char *)"nonexistent" ) == NULL )
// 		std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
// 	if ( !strcmp ( (char *)get ( config [ 0 ].error_pages , (u_char *)"404" )->value , "404.html" ) )
// 		std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
// 	if ( !strcmp ( (char *)get ( config [ 0 ].error_pages , (u_char *)"434" )->value , "/hellbound_errors/434.html" ) )
// 		std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
// 	if ( get ( config [ 0 ].routes , (u_char *)"nonexistent" ) == NULL )
// 		std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
// 	unicore_route_t *route = (unicore_route_t *)get ( config [ 0 ].routes , (u_char *)"/route" )->value;
// 	if ( !strcmp ( route->root , "/root_dir" ) )
// 		std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( !strcmp ( route->file_if_directory_request , "file" ) )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->upload_path == NULL )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->ROUTE_GET == 1 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->ROUTE_POST == 0 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->ROUTE_DELETE == 1 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->ROUTE_UPLOADS == 0 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->CGI_GET == 1 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->CGI_POST == 1 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->CGI_PYTHON == 0 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->CGI_PHP == 0 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->directory_listing == 0 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     route = (unicore_route_t *)get ( config [ 0 ].routes , (u_char *)"/" )->value;
// 	if ( !strcmp ( route->root , "/" ) )
// 		std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( !strcmp ( route->file_if_directory_request , "FILE" ) )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( !strcmp ( "uploads/videos" , route->upload_path ) )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->ROUTE_GET == 0 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->ROUTE_POST == 0 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->ROUTE_DELETE == 0 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->ROUTE_UPLOADS == 1 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->CGI_GET == 1 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->CGI_POST == 1 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->CGI_PYTHON == 1 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->CGI_PHP == 1 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( route->directory_listing == 1 )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( get ( config [ 0 ].redirection_list , (u_char *)"shitdoesntexist" ) == NULL )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( !strcmp ( "/newurl/newshit" , (char *)get ( config [ 0 ].redirection_list , (u_char *)"/oldurl/smshit" )->value ) )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
//     if ( !strcmp ( "/oui" , (char *)get ( config [ 0 ].redirection_list , (u_char *)"/another" )->value ) )
//     	std::cout << "\e[0;32mpass, ";
// 	else
// 		std::cout << "\e[0;31mfail, ";
    
    std::ifstream stream1 ( "tests/config_pass.conf" );

    unicore_config_parse ( stream1 , conf );

//     std::cout << "\n\t\t\t\t\e[1;37mREQUEST-LINE\e[0m\n";
//     std::cout << "ACCEPTS\n";
//     for ( int i = 0; i < 14; i++ )
//     {

//         b.start = REQUEST_LINE_ACCEPTS [ i ];
//         b.pos = REQUEST_LINE_ACCEPTS [ i ];
//         b.end = b.start + std::strlen ( (const char *)REQUEST_LINE_ACCEPTS [ i ] );
//         if ( unicore_http_parse_request_line ( &r , &b , conf [ 0 ] ) == 1 )
//             std::cout << "\e[0;32mpass, ";
//         else
//             std::cout << "\e[0;31mfail, ";

//     }

//     std::cout << "\e[0m\nREJECTS\n";
//     for ( int i = 0; i < 10; i++ )
//     {

//         b.start = REQUEST_LINE_REJECTS [ i ];
//         b.pos = REQUEST_LINE_REJECTS [ i ];
//         b.end = b.start + std::strlen ( (const char *)REQUEST_LINE_REJECTS [ i ] );
//         if ( unicore_http_parse_request_line ( &r , &b , conf [ 0 ] ) == -1 )
//             std::cout << "\e[0;32mpass, ";
//         else
//             std::cout << "\e[0;31mfail, ";

//     }


    u_char *request = (u_char *)"GET /route/.py/extrapath/shithtml/?name=yes HTTP/1.1\r\n";
    // u_char *request = (u_char *)"GET /servlet?;jsessionid=1234&param=value/ HTTP/1.1 \r\n";
    unicore_buf_t bb = { request , request , request + 54 };

    std::cout << unicore_http_parse_request_line ( &r , &bb , conf [ 0 ] );

    return 0;



}