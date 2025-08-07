

#include "unicore_buf.hpp"
#include "unicore_request.hpp"
#include "unicore_defines.hpp"
#include "unicore_http_parse.hpp"
#include "unicore_config_parse.hpp"

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <cstdlib>

#include <unistd.h>

#include <iostream>

int unicore_http_parse_request_line ( fsm_state_t& fsm_state , unicore_buf_t *b 
      , unicore_config_t& c )
{   

   enum state
   {

        START = 0,
        START_CR,
        START_LF,
        REQUEST_LINE_PRECEDING_SP_FIELD,
        CARRIAGE_RETURN,
        LINE_FEED,
        REQUEST_LINE_START_GET,
        REQUEST_LINE_START_POST,
        REQUEST_LINE_START_DELETE,
        GET_E_ALPHA,
        GET_T_ALPHA,
        POST_O_ALPHA,
        POST_S_ALPHA,
        POST_T_ALPHA,
        DELETE_E1_ALPHA,
        DELETE_L_ALPHA,
        DELETE_E2_ALPHA,
        DELETE_T_ALPHA,
        DELETE_E3_ALPHA,
        METHOD_VALIDATED_BY_SP,
        ORIGIN_FORM_FORWARD_SLASH,
        URI_SEGMENT,
        ORIGIN_FORM_QUESTION_MARK,
        QUERY_FORWARD_SLASH,
        QUERY_PCHAR,
        ORIGIN_FORM_VALIDATED_BY_SP,
        REQUEST_LINE_HTTP_H_ALPHA,
        REQUEST_LINE_HTTP_T1_ALPHA,
        REQUEST_LINE_HTTP_T2_ALPHA,
        REQUEST_LINE_HTTP_P_ALPHA,
        REQUEST_LINE_HTTP_FORWARD_SLASH_BEFORE_VERSION,
        REQUEST_LINE_HTTP_MAJOR_VERSION_1,
        REQUEST_LINE_HTTP_VERSION_DOT,
        REQUEST_LINE_HTTP_MINOR_VERSION_1,
        REQUEST_LINE_TRAILING_SP_FIELD

   } state;

   state = ( enum state )fsm_state.state;
   for ( fsm_state.p = b->pos; fsm_state.p <= b->end ; fsm_state.p++ )
   {

      fsm_state.state = state;
      fsm_state.ch = *fsm_state.p;

      switch ( state )
      {
      
         /* first request-line character */
         case START:
            fsm_state.primary_i = 0;
            fsm_state.secondary_i = 0;
            fsm_state.tertiary_i = 0;
            fsm_state.portion = 0;
            fsm_state.r = new unicore_request_t;
            std::memset ( fsm_state.r , 0 , sizeof ( unicore_request_t ) );
            fsm_state.r->headers = new ht;
            fsm_state.r->headers->buckets = new bucket[M];
            std::memset( fsm_state.r->headers->buckets , 0 , M * sizeof(bucket) );
            fsm_state.r->SCRIPT_NAME = NULL;
            fsm_state.r->PATH_INFO = NULL;
            fsm_state.r->PATH_TRANSLATED = NULL;
            fsm_state.r->QUERY_STRING = NULL;
            fsm_state.r->GATEWAY_INTERFACE = NULL;
            fsm_state.r->http_version = 101;
            switch ( fsm_state.ch )
            {

               case SP:
                  state = REQUEST_LINE_PRECEDING_SP_FIELD;
                  break;
               case 'G':
                  state = REQUEST_LINE_START_GET;
                  break;
               case 'P':
                  state = REQUEST_LINE_START_POST;
                  break;
               case 'D':
                  state = REQUEST_LINE_START_DELETE;
                  break;
               case CR:
                  state = START_CR;
                  break;
               default:
                  return 501; // 501 Not Implemented

            }
            break;
         
    /* possible recurring CRLF before request-line */
         case START_CR:
            switch ( fsm_state.ch )
            {

               case LF:
                  state = START_LF;
                  break;
               default:
                  return 400; // 400 Bad Request
               

            }
            break;

         case START_LF:
            switch ( fsm_state.ch )
            {

               case 'G':
                  state = REQUEST_LINE_START_GET;
                  break;
               case 'P':
                  state = REQUEST_LINE_START_POST;
                  break;
               case 'D':
                  state = REQUEST_LINE_START_DELETE;
                  break;
               case CR:
                  state = START_CR;
                  break;
               case SP:
                  state = REQUEST_LINE_PRECEDING_SP_FIELD;
                  break;
               default:
                  return 501; // 501 Not Implemented
               
            }
            break;

   /* possible recurring SP before request-line */
         case REQUEST_LINE_PRECEDING_SP_FIELD:
            switch ( fsm_state.ch )
            {

               case SP:
                  break;
               case 'G':
                  state = REQUEST_LINE_START_GET;
                  break;
               case 'P':
                  state = REQUEST_LINE_START_POST;
                  break;
               case 'D':
                  state = REQUEST_LINE_START_DELETE;
                  break;

               default:
                  return 501; // 501 Not Implemented

            }
            break;


   /* start-line terminator is a single CRLF */
   /* a single LF is permitted but bare CR isn't */
         case CARRIAGE_RETURN:
            switch ( fsm_state.ch )
            {

               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return 400; // 400 bad request

            }
            break;

         case LINE_FEED:
            b->pos = fsm_state.p;
            fsm_state.state = 0;
            return UNICORE_VALID_REQUEST_LINE_SUCCESS;

         case REQUEST_LINE_START_GET:
            fsm_state.r->REQUEST_METHOD = GET;
            switch ( fsm_state.ch )
            {

               case 'E':
                  state = GET_E_ALPHA;
                  break;
               default:
                  return 501; // 501 Not Implemented

            }
            break;

         case REQUEST_LINE_START_POST:
            fsm_state.r->REQUEST_METHOD = POST;
            switch ( fsm_state.ch )
            {

               case 'O':
                  state = POST_O_ALPHA;
                  break;
               default:
                  return 501;

            }
            break;

         case REQUEST_LINE_START_DELETE:
            fsm_state.r->REQUEST_METHOD = DELETE;
            switch ( fsm_state.ch )
            {

               case 'E':
                  state = DELETE_E1_ALPHA;
                  break;
               default:
                  return 501;

            }
            break;

         case GET_E_ALPHA:
            switch ( fsm_state.ch )
            {

               case 'T':
                  state = GET_T_ALPHA;
                  break;
               default:
                  return 501;

            }
            break;

         case GET_T_ALPHA:
            switch ( fsm_state.ch )
            {

               case SP:
                  state = METHOD_VALIDATED_BY_SP;
                  break;
               default:
                  return 501;

            }
            break;

         case POST_O_ALPHA:
            switch ( fsm_state.ch )
            {

               case 'S':
                  state = POST_S_ALPHA;
                  break;
               default:
                  return 501;

            }
            break;

         case POST_S_ALPHA:
            switch ( fsm_state.ch )
            {

               case 'T':
                  state = POST_T_ALPHA;
                  break;
               default:
                  return 501;

            }
            break;

         case POST_T_ALPHA:
            switch ( fsm_state.ch )
            {

               case SP:
                  state = METHOD_VALIDATED_BY_SP;
                  break;
               default:
                  return 501;

            }
            break;

         case DELETE_E1_ALPHA:
            switch ( fsm_state.ch )
            {

               case 'L':
                  state = DELETE_L_ALPHA;
                  break;
               default:
                  return 501;

            }
            break;

         case DELETE_L_ALPHA:
            switch ( fsm_state.ch )
            {

               case 'E':
                  state = DELETE_E2_ALPHA;
                  break;
               default:
                  return 501;

            }
            break;

         case DELETE_E2_ALPHA:
            switch ( fsm_state.ch )
            {

               case 'T':
                  state = DELETE_T_ALPHA;
                  break;
               default:
                  return 501;

            }
            break;

         case DELETE_T_ALPHA:
            switch ( fsm_state.ch )
            {

               case 'E':
                  state =  DELETE_E3_ALPHA;
                  break;
               default:
                  return 501;

            }
            break;

         case DELETE_E3_ALPHA:
            switch ( fsm_state.ch )
            {

               case SP:
                  state = METHOD_VALIDATED_BY_SP;
                  break;
               default:
                  return 400;

            }
            break;

         case METHOD_VALIDATED_BY_SP:
            switch ( fsm_state.ch )
            {

               case SP:
                  break;
               case '/':
                  fsm_state.portion = 1;
                  fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
                  fsm_state.tertiary_buf [ fsm_state.tertiary_i++ ] = fsm_state.ch;
                  state = ORIGIN_FORM_FORWARD_SLASH;
                  break;
               default:
                  return 400;

            }
            break;

         case ORIGIN_FORM_FORWARD_SLASH:
            if ( fsm_state.ch == '.' )
            {

               fsm_state.p--;
               state = URI_SEGMENT;
               break;

            }
            if ( PCHAR(fsm_state.ch) )
            {

               if ( fsm_state.primary_i > 510 or fsm_state.tertiary_i > 510  )
                  return 400;
               fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
               fsm_state.tertiary_buf [ fsm_state.tertiary_i++ ] = fsm_state.ch;
               state = URI_SEGMENT;
               break;

            }

            if ( fsm_state.portion == 3 )
            {

               fsm_state.r->PATH_INFO = new u_char [ fsm_state.primary_i - fsm_state.path_info_start + 1 ];
               for ( int i = 0, k = fsm_state.path_info_start ; k < fsm_state.primary_i ; i++, k++ )
                  fsm_state.r->PATH_INFO [ i ] = fsm_state.primary_buf [ k ];
               fsm_state.r->PATH_INFO [ fsm_state.primary_i - fsm_state.path_info_start ] = '\0';

            }

            switch ( fsm_state.ch )
            {

               case SP:
                  if ( fsm_state.portion == 1 )
                  {

                     fsm_state.buckett = get ( c.routes , (u_char *)"/" );
                     if ( fsm_state.buckett == NULL )
                        return 400;
                     fsm_state.r->route = ( unicore_route_t * )fsm_state.buckett->value;

                  }
                  state = ORIGIN_FORM_VALIDATED_BY_SP;
                  break;
               case '?':
                  std::memset ( fsm_state.secondary_buf , 0 , 512 );
                  fsm_state.secondary_i = 0;
                  state = ORIGIN_FORM_QUESTION_MARK;
                  break;
               default:
                  return 400;
               
            }
            break;

         case URI_SEGMENT:
            if ( fsm_state.ch == '.' and fsm_state.dotdot_guard )
               return 403;
            else if ( fsm_state.ch == '.' )
               fsm_state.dotdot_guard = 1;
            else if ( fsm_state.ch != '.' )
               fsm_state.dotdot_guard = 0;
            if ( PCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.tertiary_i > 510 )
                  return 400;
               fsm_state.tertiary_buf [ fsm_state.tertiary_i++ ] = fsm_state.ch;
               if ( ( fsm_state.portion == 1 or fsm_state.portion == 2 ) and fsm_state.p [ 0 ] == '.' and ( 
                  ( fsm_state.p [ 1 ] and fsm_state.p [ 1 ] == 'p' and fsm_state.p [ 2 ] and fsm_state.p [ 2 ] == 'y' ) or
                  ( fsm_state.p [ 1 ] and fsm_state.p [ 1 ] == 'p' and fsm_state.p [ 2 ] and fsm_state.p [ 2 ] == 'h' and
                           fsm_state.p [ 3 ] and fsm_state.p [ 3 ] == 'p' ) ) )
               {

                  if ( fsm_state.portion == 1 )
                  {

                     fsm_state.buckett = get ( c.routes , (u_char *)"/" );
                     if ( fsm_state.buckett == NULL )
                        return 400;
                     fsm_state.r->route = ( unicore_route_t * )fsm_state.buckett->value;

                  }
                  fsm_state.r->cgi = 1;
                  if ( fsm_state.p [ 2 ] == 'y' )
                  {

                     fsm_state.r->cgi_script_type = PYTHON;
                     for ( int i = 0 ; i < 3 ; i++, fsm_state.p++ )
                     {

                        if ( fsm_state.primary_i > 510 )
                           return 400;
                        fsm_state.primary_buf [ fsm_state.primary_i++ ] = *fsm_state.p;
                     }

                  }
                  else
                  {

                     fsm_state.r->cgi_script_type = PHP;
                     for ( int i = 0 ; i < 4 ; i++, fsm_state.p++ )
                     {

                        if ( fsm_state.primary_i > 510 )
                           return 400;
                        fsm_state.primary_buf [ fsm_state.primary_i++ ] = *fsm_state.p;
                     }

                  }
                  fsm_state.r->SCRIPT_NAME = new u_char [ fsm_state.primary_i + 1 ];
                  for ( int i = 0 ; i < fsm_state.primary_i ; i++ )
                     fsm_state.r->SCRIPT_NAME [ i ] = fsm_state.primary_buf [ i ];
                  fsm_state.r->SCRIPT_NAME [ fsm_state.primary_i ] = '\0';
                  fsm_state.portion = 3;
                  fsm_state.path_info_start = fsm_state.primary_i;
                  fsm_state.p--; // so that a character is not skipped in the next iteration

               }
               else
               {

                  if ( fsm_state.primary_i > 510 )
                     return 400;
                  fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;

               }
               break;

            }

            if ( fsm_state.portion == 1 )
            {
               
               // std::cout << "PRIMARY BUF " << primary_buf;
               // std::exit (1);
               if ( !c.routes )
               {

                  std::cout << "mamak ";

               }
               fsm_state.buckett = get ( c.routes , fsm_state.primary_buf );
               /* if second hierarchy route exists then it is part of the route component 
                     and shouldn't be in SCRIPT_NAME which also uses primary_buf */
               // std::cout << "ROUTE " << primary_buf << "\n";
               if ( fsm_state.buckett )
               {

                  std::memset ( fsm_state.primary_buf , 0 , 512 );
                  fsm_state.primary_i = 0;

               }
               else
               {
                  // std::cout << "ROUTE " << "/" << "\n";
                  fsm_state.buckett = get ( c.routes , (u_char *)"/" );
                  if ( fsm_state.buckett == NULL )
                     return 400;

               }
               fsm_state.r->route = ( unicore_route_t * )fsm_state.buckett->value;
               fsm_state.portion = 2;

            }
            else if ( fsm_state.portion == 3 and fsm_state.ch != '/' )
            {

               fsm_state.r->PATH_INFO = new u_char [ fsm_state.primary_i - fsm_state.path_info_start + 1 ];
               for ( int i = 0, k = fsm_state.path_info_start ; k < fsm_state.primary_i ; i++, k++ )
                  fsm_state.r->PATH_INFO [ i ] = fsm_state.primary_buf [ k ];
               fsm_state.r->PATH_INFO [ fsm_state.primary_i - fsm_state.path_info_start ] = '\0';

            }


            switch ( fsm_state.ch )
            {

               case SP:
                  state = ORIGIN_FORM_VALIDATED_BY_SP;
                  break;
               case '/':
                  if ( fsm_state.primary_i > 510 or fsm_state.tertiary_i > 510 )
                     return 400;
                  fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
                  fsm_state.tertiary_buf [ fsm_state.tertiary_i++ ] = fsm_state.ch;
                  state = ORIGIN_FORM_FORWARD_SLASH;
                  break;
               case '?':
                  std::memset ( fsm_state.secondary_buf , 0 , 512 );
                  fsm_state.secondary_i = 0;
                  state = ORIGIN_FORM_QUESTION_MARK;
                  break;
               default:
                  return 400;

            }
            break;

         case ORIGIN_FORM_QUESTION_MARK:
            if ( fsm_state.r->route == NULL )
            {

               fsm_state.buckett = get ( c.routes , ( u_char * )"/" );
               if ( fsm_state.buckett == NULL )
                  return 404;
               fsm_state.r->route = ( unicore_route_t * )fsm_state.buckett->value;

            }
            if ( PCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.secondary_i > 510 )
                  return 400;
               fsm_state.secondary_buf [ fsm_state.secondary_i++ ] = fsm_state.ch;
               state = QUERY_PCHAR;
               break;

            }
            switch ( fsm_state.ch )
            {

               case SP:
                  // std::cout << "ROUTE " << primary_buf;
                  fsm_state.r->QUERY_STRING = new u_char [ fsm_state.secondary_i + 1 ];
                  for ( int i = 0 ; i < fsm_state.secondary_i ; i++ )
                     fsm_state.r->QUERY_STRING [ i ] = fsm_state.secondary_buf [ i ];
                  fsm_state.r->QUERY_STRING [ fsm_state.secondary_i ] = '\0';
                  state = ORIGIN_FORM_VALIDATED_BY_SP;
                  break;
               case '/':
                  if ( fsm_state.secondary_i > 510 )
                     return 400;
                  fsm_state.secondary_buf [ fsm_state.secondary_i++ ] = fsm_state.ch;
                  state = QUERY_FORWARD_SLASH;
                  break;
               case '?':
                  if ( fsm_state.secondary_i > 510 )
                     return 400;
                  fsm_state.secondary_buf [ fsm_state.secondary_i++ ] = fsm_state.ch;
                  break;
               default:
                  return 400;

            }
            break;

         case QUERY_FORWARD_SLASH:
            if ( PCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.secondary_i > 510 )
                  return 400;
               fsm_state.secondary_buf [ fsm_state.secondary_i++ ] = fsm_state.ch;
               state = QUERY_PCHAR;
               break;

            }
            switch ( fsm_state.ch )
            {

               case SP:
                  fsm_state.r->QUERY_STRING = new u_char [ fsm_state.secondary_i + 1 ];
                  for ( int i = 0 ; i < fsm_state.secondary_i ; i++ )
                     fsm_state.r->QUERY_STRING [ i ] = fsm_state.secondary_buf [ i ];
                  fsm_state.r->QUERY_STRING [ fsm_state.secondary_i ] = '\0';
                  state = ORIGIN_FORM_VALIDATED_BY_SP;
                  break;
               case '?':
                  if ( fsm_state.secondary_i > 510 )
                     return 400;
                  fsm_state.secondary_buf [ fsm_state.secondary_i++ ] = fsm_state.ch;
                  state = ORIGIN_FORM_QUESTION_MARK;
                  break;
               case '/':
                  if ( fsm_state.secondary_i > 510 )
                     return 400;
                  fsm_state.secondary_buf [ fsm_state.secondary_i++ ] = fsm_state.ch;
                  break;
               default:
                  return 400;

            }
            break;

         case QUERY_PCHAR:
            if ( PCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.secondary_i > 510 )
                  return 400;
               fsm_state.secondary_buf [ fsm_state.secondary_i++ ] = fsm_state.ch;
               break;

            }
            
            switch ( fsm_state.ch )
            {

               case SP:
                  fsm_state.r->QUERY_STRING = new u_char [ fsm_state.secondary_i + 1 ];
                  for ( int i = 0 ; i < fsm_state.secondary_i ; i++ )
                    fsm_state.r->QUERY_STRING [ i ] = fsm_state.secondary_buf [ i ];
                  fsm_state.r->QUERY_STRING [ fsm_state.secondary_i ] = '\0';
                  state = ORIGIN_FORM_VALIDATED_BY_SP;
                  break;
               case '?':
                  if ( fsm_state.secondary_i > 510 )
                     return 400;
                  fsm_state.secondary_buf [ fsm_state.secondary_i++ ] = fsm_state.ch;
                  state = ORIGIN_FORM_QUESTION_MARK;
                  break;
               case '/':
                  if ( fsm_state.secondary_i > 510 )
                     return 400;
                  fsm_state.secondary_buf [ fsm_state.secondary_i++ ] = fsm_state.ch;
                  state = QUERY_FORWARD_SLASH;
                  break;
               default:
                  return 400;

            }
            break;

         case ORIGIN_FORM_VALIDATED_BY_SP:

            // std::cout << "ROOT " << r->route->root << "\n";
            if ( fsm_state.r->cgi == 0 and fsm_state.r->static_uri_path == NULL )
            {

               fsm_state.r->static_uri_path = new u_char [ fsm_state.primary_i + 1 ];
               for ( int i = 0 ; i < fsm_state.primary_i ; i++ )
                  fsm_state.r->static_uri_path [ i ] = fsm_state.primary_buf [ i ];
               fsm_state.r->static_uri_path [ fsm_state.primary_i ] = '\0';

            }

            if ( fsm_state.r->absolute_path == NULL )
            {

               fsm_state.r->absolute_path = new u_char [ fsm_state.tertiary_i + 1 ];
               for ( int i = 0 ; i < fsm_state.tertiary_i ; i++ )
                  fsm_state.r->absolute_path [ i ] = fsm_state.tertiary_buf [ i ];
               fsm_state.r->absolute_path [ fsm_state.tertiary_i ] = '\0';

               // std::cout << "ABSOLUTE PATH " << r->absolute_path << "?\n";

            }
            
            // std::cout << "SCRIPT_NAME " << r->SCRIPT_NAME << "\n";
            // std::cout << "PATH_INFO " << r->PATH_INFO << "\n";
            // std::cout << "QUERY_STRING " << r->QUERY_STRING << "\n";
            // if ( r->static_uri_path )
            //    std::cout << "static path " << r->static_uri_path << "\n";
            switch ( fsm_state.ch )
            {

               case SP:
                  break;
               case 'H':
                  state = REQUEST_LINE_HTTP_H_ALPHA;
                  break;
               default:
                  return 400;

            }
            break;

         case REQUEST_LINE_HTTP_H_ALPHA:
            switch ( fsm_state.ch )
            {

               case 'T':
                  state = REQUEST_LINE_HTTP_T1_ALPHA;
                  break;
               default:
                  return 400;

            }
            break;

         case REQUEST_LINE_HTTP_T1_ALPHA:
            switch ( fsm_state.ch )
            {

               case 'T':
                  state = REQUEST_LINE_HTTP_T2_ALPHA;
                  break;
               default:
                  return 400;

            }
            break;

         case REQUEST_LINE_HTTP_T2_ALPHA:
            switch ( fsm_state.ch )
            {

               case 'P':
                  state = REQUEST_LINE_HTTP_P_ALPHA;
                  break;
               default:
                  return 400;

            }
            break;

         case REQUEST_LINE_HTTP_P_ALPHA:
            switch ( fsm_state.ch )
            {

               case '/':
                  state = REQUEST_LINE_HTTP_FORWARD_SLASH_BEFORE_VERSION;
                  break;
               default:
                  return 400;

            }
            break;

         case REQUEST_LINE_HTTP_FORWARD_SLASH_BEFORE_VERSION:
            switch ( fsm_state.ch )
            {

               case '1':
                  state = REQUEST_LINE_HTTP_MAJOR_VERSION_1;
                  break;
               default:
                  return 400;

            }
            break;

         case REQUEST_LINE_HTTP_MAJOR_VERSION_1:
            switch ( fsm_state.ch )
            {

               case '.':
                  state = REQUEST_LINE_HTTP_VERSION_DOT;
                  break;
               default:
                  return 400;

            }
            break;

         case REQUEST_LINE_HTTP_VERSION_DOT:
            switch ( fsm_state.ch )
            {

               case '1':
                  state = REQUEST_LINE_HTTP_MINOR_VERSION_1;
                  break;
               default:
                  return 400;

            }
            break;

         case REQUEST_LINE_HTTP_MINOR_VERSION_1:
            switch ( fsm_state.ch )
            {

               case SP:
                  state = REQUEST_LINE_TRAILING_SP_FIELD;
                  break;
               case CR:
                  state = CARRIAGE_RETURN;
                  break;
               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return 400;

            }
            break;

         case REQUEST_LINE_TRAILING_SP_FIELD:
            switch ( fsm_state.ch )
            {

               case SP:
                  break;
               case CR:
                  state = CARRIAGE_RETURN;
                  break;
               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return 400;

            }      

      }

   }

   if ( state == LINE_FEED )
   {
   
      b->pos = fsm_state.p;
      fsm_state.state = 0;
      return UNICORE_VALID_REQUEST_LINE_SUCCESS;

   }
   fsm_state.state = state;
   return UNICORE_INCOMPLETE_REQUEST_LINE;

}

int unicore_http_parse_field_lines ( fsm_state_t& fsm_state , unicore_buf_t *b )
{

   static const u_char  lowcase[] =
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
        "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
   
   enum state
   {
         START = 0,
         FIELD_NAME_TCHAR,
         FIELD_LINE_COLON,
         FIELD_LINE_OWS_AFTER_COLON,
         FIELD_VALUE_FIRST_VCHAR,
         FIELD_VALUE_OPTIONAL_FIRST_VCHAR,
         FIELD_VALUE_OPTIONAL_SP,
         FIELD_VALUE_OPTIONAL_HT,
         FIELD_VALUE_OPTIONAL_FIRST_AND_LAST_VCHAR_SUPERPOSITION,
         FIELD_LINE_SP_AND_TERMINATING_OWS_SUPERPOSITION,
         FIELD_LINE_CR,
         FIELD_LINE_LF,
         CARRIAGE_RETURN,
         LINE_FEED
         
   } state;

   state = ( enum state )fsm_state.state;
   for ( fsm_state.p = b->pos; fsm_state.p <= b->end ; fsm_state.p++ )
   {

      fsm_state.state = state;
      fsm_state.ch = *fsm_state.p;

      switch ( state )
      {

         case START:
            // fsm_state.r->headers = new ht;
            // fsm_state.r->headers->buckets = new bucket [ M ];
            // std::memset ( fsm_state.r->headers->buckets , 0 , M );
            if ( TCHAR( fsm_state.ch ) )
            {

               // strt = p
               std::memset ( fsm_state.primary_buf , 0 , 512 );
               fsm_state.primary_i = 0;
               fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
               state = FIELD_NAME_TCHAR;
               break;

            }
            switch ( fsm_state.ch )
            {

               case CR:
                  state = CARRIAGE_RETURN;
                  break;
               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return 400;

            }
            break;

         case FIELD_NAME_TCHAR:
            if ( TCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.primary_i > 510 )
                  return 400;
               fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
               break;

            }
            switch ( fsm_state.ch )
            {

               case ':':
                  state = FIELD_LINE_COLON;
                  break;
               default:
                  return 400;

            }
            break;

         case FIELD_LINE_COLON:
            /* 

               create a copy of key for hashing
               field-lines are case insensitive so lowcase is used for conversion 

            */
            // len = p - strt - 1;
            fsm_state.key = new u_char [ fsm_state.primary_i + 1 ];
            if ( !fsm_state.key )
               std::exit ( 1 );
            // i = 0;
            for ( int i = 0; i < fsm_state.primary_i ; i++ )
            {

               fsm_state.key [ i ] = lowcase [ fsm_state.primary_buf [ i ] ];
               if ( !fsm_state.key [ i ] )
                  fsm_state.key [ i ] = fsm_state.primary_buf [ i ];

            }
            fsm_state.key [ fsm_state.primary_i ] = '\0';
            // strt = NULL;
            fsm_state.primary_i = 0;

            if ( VCHAR( fsm_state.ch ) )
            {

               // strt = p;
               std::memset ( fsm_state.primary_buf , 0 , 512 );
               fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
               state = FIELD_VALUE_FIRST_VCHAR;
               break;

            }
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = FIELD_LINE_OWS_AFTER_COLON;
                  break;
               case CR:
                  state = FIELD_LINE_CR;
                  break;
               default:
                  return 400;

            }
            break;

         case FIELD_LINE_OWS_AFTER_COLON:
            if ( VCHAR( fsm_state.ch ) )
            {

               // strt = p;
               std::memset ( fsm_state.primary_buf , 0 , 512 );
               fsm_state.primary_i = 0;
               fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
               state = FIELD_VALUE_FIRST_VCHAR;
               break;

            }
            switch ( fsm_state.ch )
            {

               /* OWS */
               case SP:
               case HT:
                  break;
               case CR:
                  state = FIELD_LINE_CR;
                  break;
               default:
                  return 400;

            }
            break;

         case FIELD_VALUE_FIRST_VCHAR:
            if ( VCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.primary_i > 510 )
                  return 400;
               fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
               state = FIELD_VALUE_OPTIONAL_FIRST_VCHAR;
               break;

            }
            switch ( fsm_state.ch )
            {

               /* OWS, look here to remove trailing whitespace from field value */
               case SP:
               case HT:
                  if ( fsm_state.primary_i > 510 )
                     return 400;
                  fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
                  state = FIELD_LINE_SP_AND_TERMINATING_OWS_SUPERPOSITION;
                  break;
               case CR:
                  state = FIELD_LINE_CR;
                  break;
               default:
                  return 400;

            }
            break;

         case FIELD_VALUE_OPTIONAL_FIRST_VCHAR:
            if ( VCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.primary_i > 510 )
                  return 400;
               fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
               state = FIELD_VALUE_OPTIONAL_FIRST_AND_LAST_VCHAR_SUPERPOSITION;
               break;

            }
            switch ( fsm_state.ch )
            {

               case SP:
                  if ( fsm_state.primary_i > 510 )
                     return 400;
                  fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
                  state = FIELD_VALUE_OPTIONAL_SP;
                  break;
               case HT:
                  if ( fsm_state.primary_i > 510 )
                     return 400;
                  fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
                  state = FIELD_VALUE_OPTIONAL_HT;
                  break;
                  /*
               addition patched for operagx request headers not 
               being ABNF compliant
               */
               case CR:
                  state = FIELD_LINE_CR;
                  break;
               default:
                  return 400;

            }
            break;

         case FIELD_VALUE_OPTIONAL_SP:
            if ( fsm_state.primary_i > 510 )
               return 400;
            fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
            if ( VCHAR( fsm_state.ch ) )
            {

               state = FIELD_VALUE_OPTIONAL_FIRST_AND_LAST_VCHAR_SUPERPOSITION;
               break;

            }

            switch ( fsm_state.ch )
            {

               case SP:
                  break;
               case HT:
                  state = FIELD_VALUE_OPTIONAL_HT;
                  break;
               default:
                  return 400;

            }
            break;

         case FIELD_VALUE_OPTIONAL_HT:
            if ( fsm_state.primary_i > 510 )
               return 400;
            fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
            if ( VCHAR( fsm_state.ch ) )
            {

               state = FIELD_VALUE_OPTIONAL_FIRST_AND_LAST_VCHAR_SUPERPOSITION;
               break;

            }
            switch ( fsm_state.ch )
            {

               case SP:
                  state = FIELD_VALUE_OPTIONAL_SP;
                  break;
               case HT:
                  break;
               default:
                  return 400;
               
            }
            break;

         case FIELD_VALUE_OPTIONAL_FIRST_AND_LAST_VCHAR_SUPERPOSITION:
            if ( VCHAR( fsm_state.ch ) )
            {
            
               if ( fsm_state.primary_i > 510 )
                  return 400;
               fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
               break;

            }
            switch ( fsm_state.ch )
            {

               /* OWS, look here to remove trailing whitespace from field value */
               case SP:
               case HT:
                  if ( fsm_state.primary_i > 510 )
                     return 400;
                  fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
                  state = FIELD_LINE_SP_AND_TERMINATING_OWS_SUPERPOSITION;
                  break;
               case CR:
                  state = FIELD_LINE_CR;
                  break;
               default:
                  return 400;

            }
            break;

         case FIELD_LINE_SP_AND_TERMINATING_OWS_SUPERPOSITION:
            if ( VCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.primary_i > 510 )
                  return 400;
               fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
               state = FIELD_VALUE_OPTIONAL_FIRST_VCHAR;
               break;

            }
            switch ( fsm_state.ch )
            {

               /* OWS, look here to remove trailing whitespace from field value */
               case SP:
               case HT:
                  if ( fsm_state.primary_i > 510 )
                     return 400;
                  fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
                  break;
               case CR:
                  state = FIELD_LINE_CR;
                  break;
               default:
                  return 400;

            }
            break;

         case FIELD_LINE_CR:
            
            if ( fsm_state.primary_i )
            {

               // len = p - strt - 1;
               // std::cout << "value primary_buf=" << primary_buf << "|" << "\n";
               fsm_state.value = new u_char [ fsm_state.primary_i + 1 ];
               if ( !fsm_state.value )
                  std::exit ( 1 );
               for ( int i = 0 ; i < fsm_state.primary_i ; i++ )
                  fsm_state.value [ i ] = fsm_state.primary_buf [ i ];
               fsm_state.value [ fsm_state.primary_i ] = '\0';
               insert ( fsm_state.r->headers , fsm_state.key , fsm_state.value );
               // strt = NULL;
               fsm_state.primary_i = 0;
               
            }
            else
               delete fsm_state.key;
            switch ( fsm_state.ch )
            {

               case LF:
                  state = FIELD_LINE_LF;
                  break;
               default:
                  return 400;

            }
            break;

         case FIELD_LINE_LF:
            if ( TCHAR( fsm_state.ch ) )
            {

               // strt = p;
               std::memset ( fsm_state.primary_buf , 0 , 512 );
               fsm_state.primary_i = 0;
               fsm_state.primary_buf [ fsm_state.primary_i++ ] = fsm_state.ch;
               state = FIELD_NAME_TCHAR;
               break;

            }
            switch ( fsm_state.ch )
            {

               case CR:
                  state = CARRIAGE_RETURN;
                  break;
               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return 400;

            }
            break;

         case CARRIAGE_RETURN:
            switch ( fsm_state.ch )
            {

               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return 400;

            }
            break;

         case LINE_FEED:
            b->pos = fsm_state.p;
            fsm_state.state = 0;
            return UNICORE_VALID_FIELD_LINES_SUCCESS;

      }

   }

   if ( state == LINE_FEED )
   {

      b->pos = fsm_state.p;
      fsm_state.state = 0;
      return UNICORE_VALID_FIELD_LINES_SUCCESS;

   }
   fsm_state.state = state;
   return UNICORE_INCOMPLETE_FIELD_LINES;

}

int unicore_http_parse_chunked_body ( fsm_state_t& fsm_state , unicore_buf_t *b )
{

   static const u_int hex_dec [ ] = {
      0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
      0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
      0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
      0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
      0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
      0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
      0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 ,
      8 , 9 , 0 , 0 , 0 , 0 , 0 , 0 ,
      0 ,10 ,11 ,12 ,13 ,14 ,15 , 0 ,
      0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
      0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
      0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
      0 ,10 ,11 ,12 ,13 ,14 ,15 , 0
    };
   
   enum state
   {

      START = 0,
      CHUNK_SIZE,
      CHUNK_EXT_BWS_BEFORE_SEMI_COLON,
      CHUNK_EXT_SEMI_COLON,
      CHUNK_EXT_BWS_AFTER_SEMI_COLON,
      CHUNK_EXT_NAME,
      CHUNK_EXT_BWS_BEFORE_EQUALS,
      CHUNK_EXT_EQUALS,
      CHUNK_EXT_BWS_AFTER_EQUALS,
      CHUNK_EXT_VALUE,
      CHUNK_CR,
      CHUNK_LF,
      CHUNK_DATA,
      CHUNK_FINAL_CR,
      CHUNK_FINAL_LF,
      LAST_CHUNK_ZERO,
      LAST_CHUNK_EXT_BWS_BEFORE_SEMI_COLON,
      LAST_CHUNK_EXT_BWS_AFTER_SEMI_COLON,
      LAST_CHUNK_EXT_SEMI_COLON,
      LAST_CHUNK_EXT_NAME,
      LAST_CHUNK_EXT_BWS_BEFORE_EQUALS,
      LAST_CHUNK_EXT_EQUALS,
      LAST_CHUNK_EXT_BWS_AFTER_EQUALS,
      LAST_CHUNK_EXT_VALUE,
      LAST_CHUNK_CR,
      LAST_CHUNK_LF,
      TRAILER_SECTION,
      CHUNKED_TOTAL_CR,
      CHUNKED_TOTAL_LF

   } state;

   state = ( enum state )fsm_state.state;
   for ( fsm_state.p = b->pos; fsm_state.p <= b->end ; fsm_state.p++ )
   {

      fsm_state.ch = *fsm_state.p;
      fsm_state.state = state;
      switch ( state )
      {

         case START:
            if ( fsm_state.ch == '0' )
               state = LAST_CHUNK_ZERO;
            else if ( ( fsm_state.ch >= '1' and fsm_state.ch <= '9' ) or ( fsm_state.ch >= 'A' and fsm_state.ch <= 'F' ) or
                        ( fsm_state.ch >= 'a' and fsm_state.ch <= 'f' ) )
            {

               state = CHUNK_SIZE;
               fsm_state.chunk_size = fsm_state.chunk_size * 16 + hex_dec [  ( int )fsm_state.ch ];
               fsm_state.hex_count++;

            }
            else
               return 400;
            break;
            
         case CHUNK_SIZE:
            if ( ( ( fsm_state.ch >= '0' and fsm_state.ch <= '9' ) or ( fsm_state.ch >= 'A' and fsm_state.ch <= 'F' ) or
                        ( fsm_state.ch >= 'a' and fsm_state.ch <= 'f' ) ) and fsm_state.hex_count < 11 )
            {

               fsm_state.chunk_size = fsm_state.chunk_size * 16 + hex_dec [  ( int )fsm_state.ch ];
               fsm_state.hex_count++;
               break;

            }
         
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = CHUNK_EXT_BWS_BEFORE_SEMI_COLON;
                  break;
               case ';':
                  state = CHUNK_EXT_SEMI_COLON;
                  break;
               case CR:
                  state = CHUNK_CR;
                  break;
               case LF:
                  state = CHUNK_LF;
                  break;
               default:
                  return 400;

            }
            break;

         case CHUNK_EXT_BWS_BEFORE_SEMI_COLON:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            else if ( fsm_state.ch == ';' )
               state = CHUNK_EXT_SEMI_COLON;
            else
               return 400;
            break;

         case CHUNK_EXT_SEMI_COLON:
            if ( TCHAR( fsm_state.ch ) )
            {

               state = CHUNK_EXT_NAME;
               break;

            }

            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = CHUNK_EXT_BWS_AFTER_SEMI_COLON;
                  break;
               default:
                  return 400;

            }
            break;

         case CHUNK_EXT_BWS_AFTER_SEMI_COLON:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            else if ( TCHAR( fsm_state.ch ) )
               state = CHUNK_EXT_NAME;
            else
               return 400;
            break;

         case CHUNK_EXT_NAME:
            if ( TCHAR( fsm_state.ch ) )
               break;

            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = CHUNK_EXT_BWS_BEFORE_EQUALS;
                  break;
               case '=':
                  state = CHUNK_EXT_EQUALS;
                  break;
               case CR:
                  state = CHUNK_CR;
                  break;
               case LF:
                  state = CHUNK_LF;
                  break;
               default:
                  return 400;

            }
            break;

         case CHUNK_EXT_BWS_BEFORE_EQUALS:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            else if ( fsm_state.ch == '=' )
               state = CHUNK_EXT_EQUALS;
            else
               return 400;
            break;

         case CHUNK_EXT_EQUALS:
            if ( TCHAR( fsm_state.ch ) )
               state = CHUNK_EXT_VALUE;
            else if ( fsm_state.ch == SP or fsm_state.ch == HT )
               state = CHUNK_EXT_BWS_AFTER_EQUALS;
            else
               return 400;
            break;

         case CHUNK_EXT_BWS_AFTER_EQUALS:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            else if ( TCHAR( fsm_state.ch ) )
               state = CHUNK_EXT_VALUE;
            else
               return 400;
            break;

         case CHUNK_EXT_VALUE:
            if ( TCHAR( fsm_state.ch ) )
               break;
            
            switch ( fsm_state.ch )
            {

               case CR:
                  state = CHUNK_CR;
                  break;
               case LF:
                  state = CHUNK_LF;
                  break;
               default:
                  return 400;

            }
            break;

         case CHUNK_CR:
            if ( fsm_state.ch == LF )
               state = CHUNK_LF;
            else
               return 400;
            break;

         case CHUNK_LF:
            fsm_state.chunk_size--;
            fsm_state.hex_count = 0;
            // write_to.append ( 1 , ch );
            // std::cout << fsm_state.ch;
            *fsm_state.file <<  fsm_state.ch;
            state = CHUNK_DATA;
            break;

         case CHUNK_DATA:
            if ( fsm_state.chunk_size )
            {

               // std::cout << fsm_state.ch;
               *fsm_state.file <<  fsm_state.ch;
               fsm_state.chunk_size--;
               // write_to.append ( 1 , ch );
               break;

            }
            

            if ( fsm_state.ch == CR )
            {

               state = CHUNK_FINAL_CR;
               break;

            }
            if ( fsm_state.ch == LF )
            {

               state = CHUNK_FINAL_LF;
               break;

            }

            if ( fsm_state.chunk_size == 0 )
            {

               // std::cout << "failed at CHUNK_DATA at char[" << ( int )fsm_state.ch << "]\n";
               return 400;


            }
            break;

         case CHUNK_FINAL_CR:
            if ( fsm_state.ch == LF )
               state = CHUNK_FINAL_LF;
            else
               return 400;
            break;

         case CHUNK_FINAL_LF:
            if ( fsm_state.ch == '0' )
               state = LAST_CHUNK_ZERO;
            else if ( ( fsm_state.ch >= '1' and fsm_state.ch <= '9' ) or ( fsm_state.ch >= 'A' and fsm_state.ch <= 'F' ) or
                        ( fsm_state.ch >= 'a' and fsm_state.ch <= 'f' ) )
            {

               state = CHUNK_SIZE;
               fsm_state.chunk_size = fsm_state.chunk_size * 16 + hex_dec [  ( int )fsm_state.ch ];
               fsm_state.hex_count++;

            }
            else
               return 400;
            break;

         case LAST_CHUNK_ZERO:
            fsm_state.chunked = 0;
            fsm_state.chunk_size = 0;
            if ( ( fsm_state.ch >= '1' and fsm_state.ch <= '9' ) or ( fsm_state.ch >= 'A' and fsm_state.ch <= 'F' ) or
                        ( fsm_state.ch >= 'a' and fsm_state.ch <= 'f' ) )
            {

               state = CHUNK_SIZE;
               fsm_state.chunk_size = fsm_state.chunk_size * 16 + hex_dec [  ( int )fsm_state.ch ];
               fsm_state.hex_count++;
               break;

            }
         
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = LAST_CHUNK_EXT_BWS_BEFORE_SEMI_COLON;
                  break;
               case '0':
                  break;
               case CR:
                  state = LAST_CHUNK_CR;
                  break;
               case LF:
                  state = LAST_CHUNK_LF;
                  break;
               case ';':
                  state = LAST_CHUNK_EXT_SEMI_COLON;
                  break;
               default:
                  return 400;

            }
            break;

         case LAST_CHUNK_EXT_BWS_BEFORE_SEMI_COLON:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            else if ( fsm_state.ch == ';' )
               state = LAST_CHUNK_EXT_SEMI_COLON;
            else
               return 400;
            break;

         case LAST_CHUNK_EXT_BWS_AFTER_SEMI_COLON:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            else if ( TCHAR( fsm_state.ch ) )
               state = LAST_CHUNK_EXT_NAME;
            else
               return 400;
            break;

         case LAST_CHUNK_EXT_SEMI_COLON:
            if ( TCHAR( fsm_state.ch ) )
               state = LAST_CHUNK_EXT_NAME;
            else if ( fsm_state.ch == SP or fsm_state.ch == HT )
               state = LAST_CHUNK_EXT_BWS_AFTER_SEMI_COLON;
            else
               return 400;
            break;

         case LAST_CHUNK_EXT_NAME:
            if ( TCHAR( fsm_state.ch ) )
               break;
            
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = LAST_CHUNK_EXT_BWS_BEFORE_EQUALS;
                  break;
               case '=':
                  state = LAST_CHUNK_EXT_EQUALS;
                  break;
               case CR:
                  state = LAST_CHUNK_CR;
                  break;
               case LF:
                  state = LAST_CHUNK_LF;
                  break;
               default:
                  return 400;

            }
            break;

         case LAST_CHUNK_EXT_BWS_BEFORE_EQUALS:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            else if ( fsm_state.ch == '=' )
               state = LAST_CHUNK_EXT_EQUALS;
            else
               return 400;
            break;

         case LAST_CHUNK_EXT_EQUALS:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               state = LAST_CHUNK_EXT_BWS_AFTER_EQUALS;
            else if ( TCHAR( fsm_state.ch ) )
               state = LAST_CHUNK_EXT_VALUE;
            else
               return 400;
            break;

         case LAST_CHUNK_EXT_BWS_AFTER_EQUALS:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            else if ( TCHAR( fsm_state.ch ) )
               state = LAST_CHUNK_EXT_VALUE;
            else
               return 400;
            break;

         case LAST_CHUNK_EXT_VALUE:
            if ( TCHAR( fsm_state.ch ) )
               break;
            else if ( fsm_state.ch == CR )
               state = LAST_CHUNK_CR;
            else if ( fsm_state.ch == LF )
               state = LAST_CHUNK_LF;
            else
               return 400;
            break;

         case LAST_CHUNK_CR:
            if ( fsm_state.ch == LF )
               state = LAST_CHUNK_LF;
            else
               return 400;
            break;

         case LAST_CHUNK_LF:
            if ( TCHAR( fsm_state.ch ) )
               state = TRAILER_SECTION;
            else if ( fsm_state.ch == CR )
               state = CHUNKED_TOTAL_CR;
            else if ( fsm_state.ch == LF )
               state = CHUNKED_TOTAL_LF;
            else
               return 400;
            break;

         case TRAILER_SECTION: // untested
            fsm_state.chunked_trailers_fsm_return = unicore_http_parse_field_lines ( fsm_state , b );
            if ( fsm_state.chunked_trailers_fsm_return == 2 )
               break;
            else if ( fsm_state.chunked_trailers_fsm_return == 1 )
            {

               b->pos = fsm_state.p;
               fsm_state.state = 0;
               return 201;

            }
            return 400;

         case CHUNKED_TOTAL_CR:
            if ( fsm_state.ch == LF )
               state = CHUNKED_TOTAL_LF;
            else
               return 400;
            break;
         
         case CHUNKED_TOTAL_LF:
            b->pos = fsm_state.p;
            fsm_state.state = 0;
            fsm_state.file->close();
            return 201;

      }

   }

   fsm_state.state = state;
   if ( state == CHUNKED_TOTAL_LF )
   {

      b->pos = fsm_state.p;
      fsm_state.state = 0;
      fsm_state.file->close();
      return 201;

   }
   fsm_state.R = 2;
   return 2;

}

int unicore_http_parse_multipart_body ( fsm_state_t& fsm_state , unicore_buf_t *b )
{
   
   static char content_disposition[] = "Content-Disposition:", content_type[] = "Content-Type:",
                     form_data[] = "form-data", name[] = "name=", filename[] = "filename=";
   static int cd_i = 20, ct_i = 13, form_data_i = 9, name_i = 5, filename_i = 9, I = 0;

   enum state
   {

      START=0,
      BOUNDARY_DASH_1,
      BOUNDARY_DASH_2,
      BOUNDARY,
      BWS,
      CR_AFTER_DASH_BOUNDARY,
      LF_AFTER_DASH_BOUNDARY,
      CONTENT_DISPOSITION,
      BWS_BEFORE_FORM_DATA,
      FORM_DATA,
      BWS_AFTER_FORM_DATA,
      SEMI_COLON_AFTER_FORM_DATA,
      BWS_AFTER_FORM_DATA_SEMI_COLON,
      NAME,
      NAME_VALUE_TCHAR,
      NAME_VALUE_DQUOTE_OPEN,
      NAME_VALUE_QUOTED_TCHAR,
      NAME_VALUE_DQUOTE_CLOSE,
      BWS_AFTER_NAME,
      SEMI_COLON_AFTER_NAME,
      BWS_BEFORE_FILENAME,
      FILENAME,
      FILENAME_VALUE_TCHAR,
      FILENAME_VALUE_DQUOTE_OPEN,
      FILENAME_VALUE_QUOTED_TCHAR,
      FILENAME_VALUE_DQUOTE_CLOSE,
      BWS_AFTER_FILENAME,
      CR_AFTER_CONTENT_DISPOSITION,
      LF_AFTER_CONTENT_DISPOSITION,
      CONTENT_TYPE,
      BWS_BEFORE_MEDIA_TYPE,
      MEDIA_TYPE_TCHAR,
      MEDIA_TYPE_FORWARD_SLASH,
      MEDIA_SUBTYPE_TCHAR,
      BWS_AFTER_MEDIA_TYPE,
      CR_AFTER_CONTENT_TYPE,
      LF_AFTER_CONTENT_TYPE,
      CR_BEFORE_BODY_PART,
      LF_BEFORE_BODY_PART,
      BODY_PART,
      CR_AFTER_BODY_PART,
      LF_AFTER_BODY_PART,
      CLOSE_DASH_1,
      CLOSE_DASH_2,

   } state;

   state = ( enum state )fsm_state.state;
   for ( fsm_state.p = b->pos; fsm_state.p <= b->end and fsm_state.content_length ; fsm_state.p++, fsm_state.content_length-- )
   {

      fsm_state.ch = *fsm_state.p;
      fsm_state.state = state;
   
      switch ( state )
      {

         case START:
            if ( fsm_state.ch == '-' )
               state = BOUNDARY_DASH_1;
            else
               return 400;
            break;

         case BOUNDARY_DASH_1:
            fsm_state.mp_r_i = 0;
            if ( fsm_state.ch == '-' )
               state = BOUNDARY_DASH_2;
            else
            {

               fsm_state.crlf_guard = std::string ( fsm_state.crlf_guard + std::string ( 1 , fsm_state.ch ) );
               *fsm_state.file << fsm_state.crlf_guard;
               fsm_state.crlf_guard.clear();
               state = BODY_PART;
               break;

            }
            fsm_state.file->close();
            std::memset ( fsm_state.content_type , 0 , 129 );
            std::memset ( fsm_state.name , 0 , 129 );
            std::memset ( fsm_state.filename , 0 , 129 );
            break;

         case BOUNDARY_DASH_2:
            for ( ; fsm_state.p <= b->end and fsm_state.mp_r_i < fsm_state.boundary_length ; fsm_state.mp_r_i++, fsm_state.p++ )
               if ( *fsm_state.p != fsm_state.boundary [ fsm_state.mp_r_i ] )
                  return 400;
            if ( fsm_state.mp_r_i != fsm_state.boundary_length )
               break;
            fsm_state.mp_r_i = 0;
            state = BOUNDARY;
            fsm_state.p--;
            break;

         case BOUNDARY:
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = BWS;
                  break;
               case CR:
                  state = CR_AFTER_DASH_BOUNDARY;
                  break;
               case '-':
                  state = CLOSE_DASH_1;
                  break;
               default:
                  return 400;

            }
            break;

         case BWS:
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  break;
               case CR:
                  state = CR_AFTER_DASH_BOUNDARY;
                  break;
               default:
                  return 400;

            }
            break;

         case CR_AFTER_DASH_BOUNDARY:
            fsm_state.mp_r_i = 0;
            if ( fsm_state.ch == LF )
               state = LF_AFTER_DASH_BOUNDARY;
            else
               return 400;
            break;

         case LF_AFTER_DASH_BOUNDARY:
            if ( fsm_state.ch == CR )
            {

               state = CR_BEFORE_BODY_PART;
               break;

            }
            for ( ; fsm_state.p <= b->end and fsm_state.mp_r_i < cd_i ; fsm_state.mp_r_i++, fsm_state.p++ )
               if ( *fsm_state.p != content_disposition [ fsm_state.mp_r_i ] )
                  return 400;
            if ( fsm_state.mp_r_i != cd_i )
               break;
            fsm_state.mp_r_i = 0;
            std::memset ( fsm_state.filename , 0 , 129 );
            state = CONTENT_DISPOSITION;
            fsm_state.p--;
            break;

         case CONTENT_DISPOSITION:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
            {

               fsm_state.mp_r_i = 0;
               state = BWS_BEFORE_FORM_DATA;
               break;

            }
            for ( ; fsm_state.p <= b->end and fsm_state.mp_r_i < form_data_i ; fsm_state.mp_r_i++, fsm_state.p++ )
               if ( *fsm_state.p != form_data [ fsm_state.mp_r_i ] )
                  return 400;
            if ( fsm_state.mp_r_i != form_data_i )
               break;
            fsm_state.mp_r_i = 0;
            state = FORM_DATA;
            fsm_state.p--;
            break;

         case BWS_BEFORE_FORM_DATA:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            for ( ; fsm_state.p <= b->end and fsm_state.mp_r_i < form_data_i ; fsm_state.mp_r_i++, fsm_state.p++ )
               if ( *fsm_state.p != form_data [ fsm_state.mp_r_i ] )
                  return 400;
            if ( fsm_state.mp_r_i != form_data_i )
               break;
            fsm_state.mp_r_i = 0;
            state = FORM_DATA;
            fsm_state.p--;
            break;

         case FORM_DATA:
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = BWS_AFTER_FORM_DATA;
                  break;
               case ';':
                  state = SEMI_COLON_AFTER_FORM_DATA;
                  break;
               case CR:
                  state = CR_AFTER_CONTENT_DISPOSITION;
                  break;
               default:
                  return 400;

            }
            break;

         case BWS_AFTER_FORM_DATA:
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  break;
               case ';':
                  fsm_state.mp_r_i = 0;
                  state = SEMI_COLON_AFTER_FORM_DATA;
                  break;
               case CR:
                  state = CR_AFTER_CONTENT_DISPOSITION;
                  break;
               default:
                  return 400;

            }
            break;
         
         case SEMI_COLON_AFTER_FORM_DATA:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
            {

               fsm_state.mp_r_i = 0;
               state = BWS_AFTER_FORM_DATA_SEMI_COLON;
               break;

            }
            for ( ; fsm_state.p <= b->end and fsm_state.mp_r_i < name_i ; fsm_state.mp_r_i++, fsm_state.p++ )
               if ( *fsm_state.p != name [ fsm_state.mp_r_i ] )
                  return 400;
            if ( fsm_state.mp_r_i != name_i )
               break;
            fsm_state.mp_r_i = 0;
            state = NAME;
            fsm_state.p--;
            break;

         case BWS_AFTER_FORM_DATA_SEMI_COLON:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            for ( ; fsm_state.p <= b->end and fsm_state.mp_r_i < name_i ; fsm_state.mp_r_i++, fsm_state.p++ )
               if ( *fsm_state.p != name [ fsm_state.mp_r_i ] )
                  return 400;
            if ( fsm_state.mp_r_i != name_i )
               break;
            fsm_state.mp_r_i = 0;
            state = NAME;
            fsm_state.p--;
            break;

         case NAME:
            if ( TCHAR( fsm_state.ch ) )
            {

               state = NAME_VALUE_TCHAR;
               fsm_state.mp_i = 0;
               fsm_state.name [ fsm_state.mp_i++ ] = fsm_state.ch;

            }
            else if ( fsm_state.ch == '"' )
               state = NAME_VALUE_DQUOTE_OPEN;
            else
               return 400;
            break;

         case NAME_VALUE_TCHAR:
            if ( TCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.mp_i >= 128 )
                  return 400;
               fsm_state.name [ fsm_state.mp_i++ ] = fsm_state.ch;
               break;

            }
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = BWS_AFTER_NAME;
                  break;
               case ';':
                  state = SEMI_COLON_AFTER_NAME;
                  break;
               case CR:
                  state = CR_AFTER_CONTENT_DISPOSITION;
                  break;
               default:
                  return 400;

            }
            break;

         case NAME_VALUE_DQUOTE_OPEN:
            if ( TCHAR( fsm_state.ch ) or fsm_state.ch == SP or fsm_state.ch == HT )
            {

               state = NAME_VALUE_QUOTED_TCHAR;
               fsm_state.mp_i = 0;
               fsm_state.name [ fsm_state.mp_i++ ] = fsm_state.ch;

            }
            else if ( fsm_state.ch == '"' )
               state = NAME_VALUE_DQUOTE_CLOSE;
            else
               return 400;
            break;
         
         case NAME_VALUE_QUOTED_TCHAR:
            if ( TCHAR( fsm_state.ch ) or fsm_state.ch == SP or fsm_state.ch == HT )
            {

               if ( fsm_state.mp_i >= 128 )
                  return 400;
               fsm_state.name [ fsm_state.mp_i++ ] = fsm_state.ch;
               break;

            }
            else if ( fsm_state.ch == '"' )
               state = NAME_VALUE_DQUOTE_CLOSE;
            else
               return 400;
            break;

         case NAME_VALUE_DQUOTE_CLOSE:
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = BWS_AFTER_NAME;
                  break;
               case ';':
                  state = SEMI_COLON_AFTER_NAME;
                  break;
               case CR:
                  state = CR_AFTER_CONTENT_DISPOSITION;
                  break;
               default:
                  return 400;

            }
            break;

         case BWS_AFTER_NAME:
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  break;
               case ';':
                  fsm_state.mp_r_i = 0;
                  state = SEMI_COLON_AFTER_NAME;
                  break;
               case CR:
                  state = CR_AFTER_CONTENT_DISPOSITION;
                  break;
               default:
                  return 400;

            }
            break;

         case SEMI_COLON_AFTER_NAME:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
            {

               fsm_state.mp_r_i = 0;
               state = BWS_BEFORE_FILENAME;
               break;

            }
            for ( ; fsm_state.p <= b->end and fsm_state.mp_r_i < filename_i ; fsm_state.mp_r_i++, fsm_state.p++ )
               if ( *fsm_state.p != filename [ fsm_state.mp_r_i ] )
                  return 400;
            if ( fsm_state.mp_r_i != filename_i )
               break;
            fsm_state.mp_r_i = 0;
            state = FILENAME;
            fsm_state.p--;
            break;

         case BWS_BEFORE_FILENAME:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            for ( ; fsm_state.p <= b->end and fsm_state.mp_r_i < filename_i ; fsm_state.mp_r_i++, fsm_state.p++ )
               if ( *fsm_state.p != filename [ fsm_state.mp_r_i ] )
                  return 400;
            if ( fsm_state.mp_r_i != filename_i )
               break;
            fsm_state.mp_r_i = 0;
            state = FILENAME;
            fsm_state.p--;
            break;

         case FILENAME:
            if ( TCHAR( fsm_state.ch ) )
            {

               state = FILENAME_VALUE_TCHAR;
               fsm_state.mp_i = 0;
               fsm_state.filename [ fsm_state.mp_i++ ] = fsm_state.ch;

            }
            else if ( fsm_state.ch == '"' )
               state = FILENAME_VALUE_DQUOTE_OPEN;
            else
               return 400;
            break;

         case FILENAME_VALUE_TCHAR:
            if ( TCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.mp_i >= 128 )
                  return 400;
               fsm_state.filename [ fsm_state.mp_i++ ] = fsm_state.ch;
               break;

            }
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = BWS_AFTER_FILENAME;
                  break;
               case CR:
                  state = CR_AFTER_CONTENT_DISPOSITION;
                  break;
               default:
                  return 400;

            }
            break;

         case FILENAME_VALUE_DQUOTE_OPEN:
            if ( TCHAR( fsm_state.ch ) or fsm_state.ch == SP or fsm_state.ch == HT )
            {

               state = FILENAME_VALUE_QUOTED_TCHAR;
               fsm_state.mp_i = 0;
               fsm_state.filename [ fsm_state.mp_i++ ] = fsm_state.ch;

            }
            else if ( fsm_state.ch == '"' )
               state = FILENAME_VALUE_DQUOTE_CLOSE;
            else
               return 400;
            break;
         
         case FILENAME_VALUE_QUOTED_TCHAR:
            if ( TCHAR( fsm_state.ch ) or fsm_state.ch == SP or fsm_state.ch == HT )
            {

               if ( fsm_state.mp_i >= 128 )
                  return 400;
               fsm_state.filename [ fsm_state.mp_i++ ] = fsm_state.ch;
               break;

            }
            if ( fsm_state.ch == '"' )
               state = FILENAME_VALUE_DQUOTE_CLOSE;
            else
               return 400;
            break;

         case FILENAME_VALUE_DQUOTE_CLOSE:
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = BWS_AFTER_FILENAME;
                  break;
               case CR:
                  state = CR_AFTER_CONTENT_DISPOSITION;
                  break;
               default:
                  return 400;

            }
            break;

         case BWS_AFTER_FILENAME:
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  break;
               case CR:
                  state = CR_AFTER_CONTENT_DISPOSITION;
                  break;
               default:
                  return 400;

            }
            break;

         case CR_AFTER_CONTENT_DISPOSITION:
            if ( fsm_state.ch == LF )
               state = LF_AFTER_CONTENT_DISPOSITION;
            else
               return 400;
            break;

         case LF_AFTER_CONTENT_DISPOSITION:
            if ( fsm_state.ch == CR )
            {

               state = CR_BEFORE_BODY_PART;
               break;

            }
            for ( ; fsm_state.p <= b->end and fsm_state.mp_r_i < ct_i ; fsm_state.mp_r_i++, fsm_state.p++ )
               if ( *fsm_state.p != content_type [ fsm_state.mp_r_i ] )
                  return 400;
            if ( fsm_state.mp_r_i != ct_i )
               break;
            fsm_state.mp_r_i = 0;
            std::memset ( fsm_state.content_type , 0 , 129 );
            state = CONTENT_TYPE;
            fsm_state.p--;
            break;

         case CONTENT_TYPE:
            if ( TCHAR( fsm_state.ch ) )
            {

               state = MEDIA_TYPE_TCHAR;
               fsm_state.mp_i = 0;
               fsm_state.content_type [ fsm_state.mp_i++ ] = fsm_state.ch;

            }
            else if ( fsm_state.ch == SP or fsm_state.ch == HT )
               state = BWS_BEFORE_MEDIA_TYPE;
            else
               return 400;
            break;

         case BWS_BEFORE_MEDIA_TYPE:
            if ( fsm_state.ch == SP or fsm_state.ch == HT )
               break;
            if ( TCHAR( fsm_state.ch ) )
            {

               state = MEDIA_TYPE_TCHAR;
               fsm_state.mp_i = 0;
               fsm_state.content_type [ fsm_state.mp_i++ ] = fsm_state.ch;

            }
            else
               return 400;
            break;

         case MEDIA_TYPE_TCHAR:
            if ( TCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.mp_i >= 128 )
                  return 400;
               fsm_state.content_type [ fsm_state.mp_i++ ] = fsm_state.ch;
               break;

            }
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = BWS_AFTER_MEDIA_TYPE;
                  break;
               case '/':
                  if ( fsm_state.mp_i >= 128 )
                     return 400;
                  fsm_state.content_type [ fsm_state.mp_i++ ] = fsm_state.ch;
                  state = MEDIA_TYPE_FORWARD_SLASH;
                  break;
               case CR:
                  state = CR_AFTER_CONTENT_TYPE;
                  break;
               default:
                  return 400;

            }
            break;

         case MEDIA_TYPE_FORWARD_SLASH:
            if ( TCHAR( fsm_state.ch ) )
            {

               state = MEDIA_SUBTYPE_TCHAR;
               if ( fsm_state.mp_i >= 128 )
                  return 400;
               fsm_state.content_type [ fsm_state.mp_i++ ] = fsm_state.ch;

            }
            else
               return 400;
            break;

         case MEDIA_SUBTYPE_TCHAR:
            if ( TCHAR( fsm_state.ch ) )
            {

               if ( fsm_state.mp_i >= 128 )
                  return 400;
               fsm_state.content_type [ fsm_state.mp_i++ ] = fsm_state.ch;
               break;

            }
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  state = BWS_AFTER_MEDIA_TYPE;
                  break;
               case CR:
                  state = CR_AFTER_CONTENT_TYPE;
                  break;
               default:
                  return 400;

            }
            break;

         case BWS_AFTER_MEDIA_TYPE:
            switch ( fsm_state.ch )
            {

               case SP:
               case HT:
                  break;
               case CR:
                  state = CR_AFTER_CONTENT_TYPE;
                  break;
               default:
                  return 400;

            }
            break;

         case CR_AFTER_CONTENT_TYPE:
            if ( fsm_state.ch == LF )
               state = LF_AFTER_CONTENT_TYPE;
            else
               return 400;
            break;
         
         case LF_AFTER_CONTENT_TYPE:
            if ( fsm_state.ch == CR )
               state = CR_BEFORE_BODY_PART;
            else
               return 400;
            break;

         case CR_BEFORE_BODY_PART:
            if ( fsm_state.ch == LF )
               state = LF_BEFORE_BODY_PART;
            else
               return 400;
            break;

         case LF_BEFORE_BODY_PART:
            state = BODY_PART;
            if ( fsm_state.r->route->upload_path and fsm_state.r->route->ROUTE_POST and fsm_state.r->REQUEST_METHOD == POST and !fsm_state.redirect_guard )
            {

               if ( fsm_state.mimes)
               {


                  if ( !*fsm_state.filename )
                  {

                     if ( *fsm_state.name and !*fsm_state.content_type )
                        fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + std::string ( fsm_state.name ) + ".txt" , std::ios::app ); // generate random filename
                     else if ( *fsm_state.name and *fsm_state.content_type )
                     {

                        fsm_state.selected_mime_type = get ( fsm_state.mimes , ( u_char * )fsm_state.content_type );
                        if ( fsm_state.selected_mime_type )
                           fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + std::string ( fsm_state.name ) + std::string ( ( char * )fsm_state.selected_mime_type->value ) , std::ios::app ); // generate random filename
                        else
                           fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + std::string ( fsm_state.name ) + ".txt" , std::ios::app ); // generate random filename

                     }
                     else if ( !*fsm_state.name and *fsm_state.content_type )
                     {
                           
                        fsm_state.selected_mime_type = get ( fsm_state.mimes , ( u_char * )fsm_state.content_type );
                        if ( fsm_state.selected_mime_type )
                           fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + std::string ( 1 , ( I++ + 48 ) ) + "multipart" + std::string ( ( char * )fsm_state.selected_mime_type->value ) , std::ios::app );

                     }
                     else
                        fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + std::string ( 1 , ( I++ + 48 ) ) + "multipart.txt" , std::ios::app ); // generate random filename

                  }
                  else
                     fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + fsm_state.filename , std::ios::app );

               }
               else
                  fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + fsm_state.filename , std::ios::app );
               
               if ( !fsm_state.file->is_open() )
                  return 404;
            }
            *fsm_state.file << fsm_state.ch;
            break;
         
         case BODY_PART:
            if ( fsm_state.ch == CR )
            {

               fsm_state.crlf_guard = std::string ( 1 , fsm_state.ch );
               state = CR_AFTER_BODY_PART;

            }
            else
               *fsm_state.file << fsm_state.ch;
            break;

         case CR_AFTER_BODY_PART:
            if ( fsm_state.ch == LF )
            {

               fsm_state.crlf_guard = std::string ( fsm_state.crlf_guard + std::string ( 1 , fsm_state.ch ) );
               state = LF_AFTER_BODY_PART;

            }
            else
            {

               fsm_state.crlf_guard = std::string ( fsm_state.crlf_guard + std::string ( 1 , fsm_state.ch ) );
               *fsm_state.file << fsm_state.crlf_guard;
               fsm_state.crlf_guard.clear();
               state = BODY_PART;


            }
            break;
         
         case LF_AFTER_BODY_PART:
            if ( fsm_state.ch == '-' )
            {

               fsm_state.crlf_guard = std::string ( fsm_state.crlf_guard + std::string ( 1 , fsm_state.ch ) );
               state = BOUNDARY_DASH_1;

            }
            else
            {

               fsm_state.crlf_guard = std::string ( fsm_state.crlf_guard + std::string ( 1 , fsm_state.ch ) );
               *fsm_state.file << fsm_state.crlf_guard;
               fsm_state.crlf_guard.clear();
               state = BODY_PART;

            }
            break;

         case CLOSE_DASH_1:
            if ( fsm_state.ch == '-' )
               state = CLOSE_DASH_2;
            else
               return 400;
            break;

         case CLOSE_DASH_2:
            fsm_state.state = 0;
            b->pos = fsm_state.p;
            return 201;         

      }

   }

   fsm_state.state = state;
   if ( state == CLOSE_DASH_2 )
   {

      std::memset ( fsm_state.filename , 0 , 129 );
      std::memset ( fsm_state.name , 0 , 129 );
      std::memset ( fsm_state.content_type , 0 , 129 );
      fsm_state.state = 0;
      b->pos = fsm_state.p;
      return 201;

   }
   fsm_state.R = 2;
   return 2;

}

int unicore_http_parse_message_body ( fsm_state_t& fsm_state , unicore_buf_t *b )
{

   bucket *transfer_encoding = get ( fsm_state.r->headers , ( u_char * )"transfer-encoding" );
   bucket *content_type = get ( fsm_state.r->headers , ( u_char * )"content-type" );
   bucket *content_length = get ( fsm_state.r->headers , ( u_char * )"content-length" );
   static char   *content_type_default_postman_form = ( char * )"multipart/form-data; boundary=";
   bucket *selected_mime_type;
   int    content_len;
   char   *content_type_str;
   static int I = 0;
   int i = 0, j = 0;


   if ( transfer_encoding and !std::strcmp ( "chunked" , ( char * )transfer_encoding->value ) )
   {

      if ( content_length )
         return 400;
      if ( content_type )
      {

         if ( !std::strncmp ( "multipart" , ( char * )content_type->value , 9 ) )
            return 400;

      }
      if ( fsm_state.r->route->upload_path and fsm_state.r->route->ROUTE_POST and fsm_state.r->REQUEST_METHOD == POST and !fsm_state.redirect_guard )
      {

         if ( !fsm_state.file->is_open() )
         {

            if ( content_type )
            {

               selected_mime_type = get ( fsm_state.mimes , ( u_char * )content_type->value );
               if ( selected_mime_type )
                  fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + std::string ( 1 , ( I++ + 48 ) ) + "chunked" + std::string ( ( char * )selected_mime_type->value ) );
               else
                  fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + std::string ( 1 , ( I++ + 48 ) ) + "chunked.txt" , std::ios::app );
            }
            else
               fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + std::string ( 1 , ( I++ + 48 ) ) + "chunked.txt" , std::ios::app );

         }
         if ( !fsm_state.file->is_open() )
            return 404;

      }
      return unicore_http_parse_chunked_body ( fsm_state , b );

   }

   else if ( content_length )
   {
      
      content_len = std::atoi ( ( char * )content_length->value );
      if ( (size_t)content_len > fsm_state.mcms )
         return 400;
      if ( content_len <= 0 )
         return 200;
      if ( fsm_state.content_length == 0 )
         fsm_state.content_length = content_len;

      if ( content_type )
      {

         content_type_str = ( char * )content_type->value;
         for ( i = 0 ; i < 30 ; i++ )
            if ( content_type_default_postman_form [ i ] != content_type_str [ i ] )
               break;
         if ( i == 30 )
         {

            for ( j = 0 ; content_type_str [ i ] ; i++, j++ )
            {

               if ( j == 70 )
                  return 400;
               fsm_state.boundary [ j ] = content_type_str [ i ]; 

            }
            fsm_state.boundary_length = j;
            return unicore_http_parse_multipart_body ( fsm_state , b );

         }
         else
         {

            if ( fsm_state.r->cgi )
            {

               fsm_state.r->route->message_body.clear();
               for ( fsm_state.p = b->pos; fsm_state.p <= b->end ; fsm_state.p++ )
               {

                  if ( fsm_state.content_length == 0 )
                  {

                     b->pos = fsm_state.p;
                     return 200;

                  }
                  fsm_state.r->route->message_body.append ( 1 , ( char )*fsm_state.p );
                  fsm_state.content_length--;
                  

               }
               if ( fsm_state.content_length == 0 )
               {

                     b->pos = fsm_state.p;
                     return 200;


               }
               fsm_state.R = 2;
               return 2;

            }
            else
            {

               if ( fsm_state.r->route->upload_path and fsm_state.r->route->ROUTE_POST and fsm_state.r->REQUEST_METHOD == POST and !fsm_state.redirect_guard )
               {

                  if ( !fsm_state.file->is_open() )
                  {

                     if ( content_type )
                     {


                        selected_mime_type = get ( fsm_state.mimes , ( u_char * )content_type->value );
                        if ( selected_mime_type )
                           fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + std::string ( 1 , ( I++ + 48 ) ) + "normal" + std::string ( ( char * )selected_mime_type->value ) );
                        else
                           fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + std::string ( 1 , ( I++ + 48 ) ) + "normal.txt" , std::ios::app );
                        
                     }
                     else
                        fsm_state.file->open ( "./_ROOT_/" + std::string ( fsm_state.r->route->root ) +"/" + std::string( fsm_state.r->route->upload_path ) + "/" + std::string ( 1 , ( I++ + 48 ) ) + "normal.txt" , std::ios::app );
                     if ( !fsm_state.file->is_open() )
                        std::cerr << "Error opening file for writing: " << fsm_state.r->route->upload_path << "\n";
                  }
                  if ( !fsm_state.file->is_open() )
                     return 404;
               }

               for ( fsm_state.p = b->pos; fsm_state.p <= b->end ; fsm_state.p++ )
               {

                  if ( fsm_state.content_length == 0 )
                  {

                     b->pos = fsm_state.p;
                     fsm_state.file->close ();
                     return 201;

                  }
                  *fsm_state.file << *fsm_state.p;
                  fsm_state.content_length--;

               }
               if ( fsm_state.content_length == 0 )
               {

                  fsm_state.file->close ();
                  b->pos = fsm_state.p;
                  return 201;


               }
               fsm_state.R = 2;
               return 2;

            }

         }

      }
      
   }

   b->pos = fsm_state.p;
   return 200;

}
