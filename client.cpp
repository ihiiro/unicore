


#include <netinet/in.h>
#include <sys/socket.h>


int main ()
{

    int cfd = socket ( AF_INET , SOCK_STREAM , 0 );
    struct sockaddr_in addr = { AF_INET , htons( 8000 ) , htonl( 2130706433 ) };
    
    connect ( cfd , ( struct sockaddr * )&addr , sizeof ( addr ) );

    send ( cfd , "hello there server, echo this back\n" ,
    36 , 0 );

}