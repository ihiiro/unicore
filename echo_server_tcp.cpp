#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>

#include <iostream>

int main ( int argc , char **argv )
{

    // listen socket ( inactive )
    int sfd = socket ( AF_INET , SOCK_STREAM , 0 );
    if ( sfd == -1 )
    {

        std::cout << "listen socket: creation failed\n";
        return 1;
       
    }
    
    struct sockaddr_in addr = { AF_INET , htons( 8000 ) , htonl( 2130706433 ) };
    if ( bind ( sfd , ( struct sockaddr * ) &addr , sizeof ( addr ) ) == -1 )
    {

        std::cout << "listen socket: bind failed\n";
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
    std::cout << "client ip int: " << peer_addr.sin_addr.s_addr;
    std::cout << "\n";

    char buf [ 512 ];
    memset ( buf , 0 , sizeof ( buf ) );
    recv ( cfd , buf , 512 , 0 );

    std::cout << "echo: " << buf;


}