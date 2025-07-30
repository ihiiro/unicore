#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>

#include <iostream>

#include <unistd.h>

#include <fstream>

#include <vector>

#include "../core/unicore_request.hpp"
#include "../core/unicore_http_parse.hpp"

int main ( )
{

    // listen socket ( inactive )
    int sfd = socket ( AF_INET , SOCK_STREAM , 0 );
    if ( sfd == -1 )
    {

        std::cout << "listen socket: creation failed\n";
        return 1;
       
    }
    
    struct sockaddr_in addr = { 0 , AF_INET , htons( 8000 ) , {htonl( 2130706433 )} , "" };
    if ( bind ( sfd , ( struct sockaddr * ) &addr , sizeof ( addr ) ) == -1 )
    {

        std::cout << "listen socket: bind failed\n" << strerror ( errno );
        return 1;

    }

    // listen
    if ( listen ( sfd , 1 ) == -1 )
    {

        std::cout << "listen socket: listen failed\n";
        return 1;

    }

    // client socket ( connection socket )
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_size = sizeof ( peer_addr );
    int cfd = accept ( sfd , ( struct sockaddr * )&peer_addr , &peer_addr_size );
    if ( cfd == -1 )
    {

        std::cout << "listen socket: accept failed\n";
        return 1;

    }
    // std::cout << "client ip int: " << peer_addr.sin_addr.s_addr;
    // std::cout << "\n";
    std::ifstream stream ( "tests/config_pass.conf" );
    std::vector < unicore_config_t > c; 
    unicore_config_parse ( stream , c );

    while ( 1 )
    {

        u_char buf [ 4096 ];
        memset ( buf , 0 , sizeof ( buf ) );



        recv ( cfd , buf , 4096 , 0 );
        unicore_request_t r;
        std::memset ( &r , 0 , sizeof ( unicore_request_t ) );
        unicore_buf_t b = { buf , buf , buf + 4096 };


        if ( unicore_http_parse_request_line ( &r , &b , c [ 0 ] ) != -1 )
            std::cout << "request line parsed\n";

        std::cout << "ROOT " << r.route->root << "\n";
        std::cout << "STATIC PATH => " << r.static_uri_path << "\n";
        std::cout << "SCRIPT NAME => " << r.SCRIPT_NAME << "\n";
        std::cout << "PATH INFO => " << r.PATH_INFO << "\n";
        std::cout << "PATH TRANSLATED => " << r.PATH_TRANSLATED << "\n";
        std::cout << "QUERY STRING => " << r.QUERY_STRING << "\n";
        std::cout << "GATEWAY INTERFACE => " << r.GATEWAY_INTERFACE << "\n";
        std::cout << "REQUEST METHOD => " << r.REQUEST_METHOD << "\n";
        std::cout << "HTTP VERSION => " << r.http_version << "\n";
        std::cout << "CGI => " << r.cgi << "\n";
        std::cout << "CGI SCRIPT TYPE => " << r.cgi_script_type << "\n";

        r.headers = new ht;
        r.headers->buckets = new bucket [ M ];
        std::memset ( r.headers->buckets , 0 , M * sizeof ( bucket ) );
        if ( unicore_http_parse_field_lines ( &r , &b ) != -1 )
            std::cout << "field lines parsed\n";

    }
    

    // bucket *rb = get ( c [ 0 ].routes , ( u_char * )"/routes" );
    // unicore_route_t *rr = (unicore_route_t *)rb->value;

    // std::cout << "root " << rr->root;
    // return 1;

    // std::cout << buf;
    // std::cout << "\n";
    


    // std::cout << send ( cfd , "HTTP/1.1 200 \r\ntransfer-encoding:chunked\r\n\r\n"
    //                                         "5\r\nhello\r\n" , 54 , 0) << std::endl;

    // sleep ( 2 );

    // // std::cout << "here";
    // std::cout << send ( cfd , "3\r\nbye\r\n" , 8 , 0 );

    // sleep ( 2 );

    // std::cout << send ( cfd , "0\r\n\r\n" , 5 , 0 );


    // std::cout << write ( cfd , "0\r\n\r\n" , 6 , 0 );

    // recv ( cfd , buf , 512 , 0 );

    // close ( sfd );
    

    // std::cout << buf;

    close ( sfd );

    // while ( 1 );


}