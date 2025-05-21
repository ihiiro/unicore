

#include "unicore_buf.hpp"
#include "unicore_request.hpp"
#include "unicore_defines.hpp"
#include "unicore_http_parse.hpp"
#include "unicore_config_parse.hpp"

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <cstdlib>


#include <iostream>

int unicore_http_parse_request_line ( unicore_request_t *r , unicore_buf_t *b , unicore_config_t *c )
{

   (void)b;
   (void)r;
   u_char ch, *p, primary_buf [ 512 ];
   int primary_i = 0, path_info_start, portion = 0;
   std::memset ( primary_buf , 0 , 512 );
   std::memset ( r , 0 , sizeof ( unicore_request_t ) );
   r->SCRIPT_NAME = (u_char *)"";
   r->PATH_INFO = (u_char *)"";
   r->PATH_TRANSLATED = (u_char *)"";
   r->QUERY_STRING = (u_char *)"";
   r->GATEWAY_INTERFACE = (u_char *)"CGI/1.1";

   enum
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

   state = START;
   for ( p = b->pos; p <= b->end ; p++ )
   {
      ch = *p;

      switch ( state )
      {
      
         /* first request-line character */
         case START:
            switch ( ch )
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
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;
         
    /* possible recurring CRLF before request-line */
         case START_CR:
            switch ( ch )
            {

               case LF:
                  state = START_LF;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;
               

            }
            break;

         case START_LF:
            switch ( ch )
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
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;
               
            }
            break;

   /* possible recurring SP before request-line */
         case REQUEST_LINE_PRECEDING_SP_FIELD:
            switch ( ch )
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
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

            switch ( ch )
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
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

   /* start-line terminator is a single CRLF */
   /* a single LF is permitted but bare CR isn't */
         case CARRIAGE_RETURN:
            switch ( ch )
            {

               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case LINE_FEED:
            b->pos = p;
            return UNICORE_VALID_REQUEST_LINE_SUCCESS;

         case REQUEST_LINE_START_GET:
            r->REQUEST_METHOD = GET;
            switch ( ch )
            {

               case 'E':
                  state = GET_E_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case REQUEST_LINE_START_POST:
            r->REQUEST_METHOD = POST;
            switch ( ch )
            {

               case 'O':
                  state = POST_O_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case REQUEST_LINE_START_DELETE:
            r->REQUEST_METHOD = DELETE;
            switch ( ch )
            {

               case 'E':
                  state = DELETE_E1_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case GET_E_ALPHA:
            switch ( ch )
            {

               case 'T':
                  state = GET_T_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case GET_T_ALPHA:
            switch ( ch )
            {

               case SP:
                  state = METHOD_VALIDATED_BY_SP;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case POST_O_ALPHA:
            switch ( ch )
            {

               case 'S':
                  state = POST_S_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case POST_S_ALPHA:
            switch ( ch )
            {

               case 'T':
                  state = POST_T_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case POST_T_ALPHA:
            switch ( ch )
            {

               case SP:
                  state = METHOD_VALIDATED_BY_SP;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case DELETE_E1_ALPHA:
            switch ( ch )
            {

               case 'L':
                  state = DELETE_L_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case DELETE_L_ALPHA:
            switch ( ch )
            {

               case 'E':
                  state = DELETE_E2_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case DELETE_E2_ALPHA:
            switch ( ch )
            {

               case 'T':
                  state = DELETE_T_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case DELETE_T_ALPHA:
            switch ( ch )
            {

               case 'E':
                  state =  DELETE_E3_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case DELETE_E3_ALPHA:
            switch ( ch )
            {

               case SP:
                  state = METHOD_VALIDATED_BY_SP;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case METHOD_VALIDATED_BY_SP:
            switch ( ch )
            {

               case SP:
                  break;
               case '/':
                  portion = 1;
                  primary_buf [ primary_i++ ] = ch;
                  state = ORIGIN_FORM_FORWARD_SLASH;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case ORIGIN_FORM_FORWARD_SLASH:
            if ( PCHAR(ch) )
            {

               if ( primary_i > 510 )
                  return UNICORE_INVALID_REQUEST_LINE_ERROR; // or specific error
               primary_buf [ primary_i++ ] = ch;
               state = URI_SEGMENT;
               break;

            }
            if (  portion == 1 )
            {

               // std::cout << primary_buf << "\n";
               r->route = (unicore_route_t *)get ( c->routes , (u_char *)"/" );
               if ( r->route == NULL )
                  return UNICORE_INVALID_REQUEST_LINE_ERROR; // or specific error
               portion = 2;
               std::memset ( primary_buf , 0 , 512 );
               primary_i = 0;
               primary_buf [ primary_i++ ] = ch;

            }


            switch ( ch )
            {

               case SP:
                  state = ORIGIN_FORM_VALIDATED_BY_SP;
                  break;
               case '?':
                  state = ORIGIN_FORM_QUESTION_MARK;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;
               
            }
            break;

         case URI_SEGMENT:
            if ( PCHAR( ch ) )
            {

               if ( primary_i > 510 )
                  return UNICORE_INVALID_REQUEST_LINE_ERROR; // or specific error
               primary_buf [ primary_i++ ] = ch;
               if ( p [ 0 ] == '.' and ( 
                  ( p [ 1 ] and p [ 1 ] == 'p' and p [ 2 ] and p [ 2 ] == 'y' ) or
                  ( p [ 1 ] and p [ 1 ] == 'p' and p [ 2 ] and p [ 2 ] == 'h' and
                           p [ 3 ] and p [ 3 ] == 'p' ) ) )
               {

                  if ( p [ 2 ] == 'y' )
                  {

                     r->cgi_script_type = PYTHON;
                     primary_buf [ primary_i++ ] = *( p++ );
                     primary_buf [ primary_i++ ] = *( p++ );

                  }
                  else
                  {

                     r->cgi_script_type = PHP;
                     primary_buf [ primary_i++ ] = *( p++ );
                     primary_buf [ primary_i++ ] = *( p++ );
                     primary_buf [ primary_i++ ] = *( p++ );

                  }
                  r->SCRIPT_NAME = new u_char [ primary_i + 1 ];
                  for ( int i = 0 ; i < primary_i ; i++ )
                     r->SCRIPT_NAME [ i ] = primary_buf [ i ];
                  r->SCRIPT_NAME [ primary_i ] = '\0';
                  portion = 3;
                  path_info_start = primary_i;
                  std::cout << "SCRIPT_NAME " << r->SCRIPT_NAME << "\n";

               }
               break;

            }

            if ( portion == 1 )
            {
               
               // std::cout << primary_buf << "\n";
               r->route = (unicore_route_t *)get ( c->routes , primary_buf );
               if ( r->route == NULL )
               {
                  r->route = (unicore_route_t *)get ( c->routes , (u_char *)"/" );
                  if ( r->route == NULL )
                     return UNICORE_INVALID_REQUEST_LINE_ERROR; // or specific error

               }
               portion = 2;
               std::memset ( primary_buf , 0 , 512 );
               primary_i = 0;
               primary_buf [ primary_i++ ] = ch;



            }
            else if ( portion == 3 and ch != '/' )
            {

               r->PATH_INFO = new u_char [ primary_i - path_info_start + 1 ];
               for ( int i = 0, k = path_info_start ; k < primary_i ; i++, k++ )
                  r->PATH_INFO [ i ] = primary_buf [ k ];
               r->PATH_INFO [ primary_i - path_info_start ] = '\0';

               std::cout << "PATH_INFO " << r->PATH_INFO << "\n";

            }


            switch ( ch )
            {

               case SP:
                  state = ORIGIN_FORM_VALIDATED_BY_SP;
                  break;
               case '/':
                  if ( primary_i > 510 )
                     return UNICORE_INVALID_REQUEST_LINE_ERROR; // or specific error
                  primary_buf [ primary_i++ ] = ch;
                  state = ORIGIN_FORM_FORWARD_SLASH;
                  break;
               case '?':
                  state = ORIGIN_FORM_QUESTION_MARK;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case ORIGIN_FORM_QUESTION_MARK:
            if ( PCHAR( ch ) )
            {

               state = QUERY_PCHAR;
               break;

            }
            switch ( ch )
            {

               case SP:
                  state = ORIGIN_FORM_VALIDATED_BY_SP;
                  break;
               case '/':
                  state = QUERY_FORWARD_SLASH;
                  break;
               case '?':
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case QUERY_FORWARD_SLASH:
            if ( PCHAR(ch) )
            {

               state = QUERY_PCHAR;
               break;

            }
            switch ( ch )
            {

               case SP:
                  state = ORIGIN_FORM_VALIDATED_BY_SP;
                  break;
               case '?':
                  state = ORIGIN_FORM_QUESTION_MARK;
                  break;
               case '/':
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case QUERY_PCHAR:
            if ( PCHAR( ch ) )
               break;
            
            switch ( ch )
            {

               case SP:
                  state = ORIGIN_FORM_VALIDATED_BY_SP;
                  break;
               case '?':
                  state = ORIGIN_FORM_QUESTION_MARK;
                  break;
               case '/':
                  state = QUERY_FORWARD_SLASH;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case ORIGIN_FORM_VALIDATED_BY_SP:
            switch ( ch )
            {

               case SP:
                  break;
               case 'H':
                  state = REQUEST_LINE_HTTP_H_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case REQUEST_LINE_HTTP_H_ALPHA:
            switch ( ch )
            {

               case 'T':
                  state = REQUEST_LINE_HTTP_T1_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case REQUEST_LINE_HTTP_T1_ALPHA:
            switch ( ch )
            {

               case 'T':
                  state = REQUEST_LINE_HTTP_T2_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case REQUEST_LINE_HTTP_T2_ALPHA:
            switch ( ch )
            {

               case 'P':
                  state = REQUEST_LINE_HTTP_P_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case REQUEST_LINE_HTTP_P_ALPHA:
            switch ( ch )
            {

               case '/':
                  state = REQUEST_LINE_HTTP_FORWARD_SLASH_BEFORE_VERSION;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case REQUEST_LINE_HTTP_FORWARD_SLASH_BEFORE_VERSION:
            switch ( ch )
            {

               case '1':
                  state = REQUEST_LINE_HTTP_MAJOR_VERSION_1;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case REQUEST_LINE_HTTP_MAJOR_VERSION_1:
            switch ( ch )
            {

               case '.':
                  state = REQUEST_LINE_HTTP_VERSION_DOT;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case REQUEST_LINE_HTTP_VERSION_DOT:
            switch ( ch )
            {

               case '1':
                  state = REQUEST_LINE_HTTP_MINOR_VERSION_1;
                  break;
               default:
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case REQUEST_LINE_HTTP_MINOR_VERSION_1:
            switch ( ch )
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
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }
            break;

         case REQUEST_LINE_TRAILING_SP_FIELD:
            switch ( ch )
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
                  return UNICORE_INVALID_REQUEST_LINE_ERROR;

            }      

      }

   }

   return UNICORE_INVALID_REQUEST_LINE_ERROR;

}

int unicore_http_parse_field_lines ( unicore_request_t *r , unicore_buf_t *b )
{

   (void)b;
   (void)r;
   u_char ch, *p, *key, *value, *strt = NULL;
   uint64_t len, i;
   static u_char  lowcase[] =
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
        "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
   
   enum
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

   state = START;
   for ( p = b->pos; p <= b->end ; p++ )
   {

      ch = *p;

      switch ( state )
      {

         case START:
            if ( TCHAR( ch ) )
            {

               strt = p;
               state = FIELD_NAME_TCHAR;
               break;

            }
            switch ( ch )
            {

               case CR:
                  state = CARRIAGE_RETURN;
                  break;
               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case FIELD_NAME_TCHAR:
            if ( TCHAR( ch ) )
               break;
            switch ( ch )
            {

               case ':':
                  state = FIELD_LINE_COLON;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case FIELD_LINE_COLON:
            /* 

               create a copy of key for hashing
               field-lines are case insensitive so lowcase is used for conversion 

            */
            len = p - strt - 1;
            key = new u_char [ len + 1 ];
            if ( !key )
               std::exit ( 1 );
            i = 0;
            for (; i < len ; i++ )
            {

               key [ i ] = lowcase [ strt [ i ] ];
               if ( !key [ i ] )
                  key [ i ] = strt [ i ];

            }
            key [ i ] = '\0';
            strt = NULL;

            if ( VCHAR( ch ) )
            {

               strt = p;
               state = FIELD_VALUE_FIRST_VCHAR;
               break;

            }
            switch ( ch )
            {

               case SP:
               case HT:
                  state = FIELD_LINE_OWS_AFTER_COLON;
                  break;
               case CR:
                  state = FIELD_LINE_CR;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case FIELD_LINE_OWS_AFTER_COLON:
            if ( VCHAR( ch ) )
            {

               strt = p;
               state = FIELD_VALUE_FIRST_VCHAR;
               break;

            }
            switch ( ch )
            {

               /* OWS */
               case SP:
               case HT:
                  break;
               case CR:
                  state = FIELD_LINE_CR;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case FIELD_VALUE_FIRST_VCHAR:
            if ( VCHAR( ch ) )
            {

               state = FIELD_VALUE_OPTIONAL_FIRST_VCHAR;
               break;

            }
            switch ( ch )
            {

               /* OWS */
               case SP:
               case HT:
                  state = FIELD_LINE_SP_AND_TERMINATING_OWS_SUPERPOSITION;
                  break;
               case CR:
                  state = FIELD_LINE_CR;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case FIELD_VALUE_OPTIONAL_FIRST_VCHAR:
            if ( VCHAR( ch ) )
            {

               state = FIELD_VALUE_OPTIONAL_FIRST_AND_LAST_VCHAR_SUPERPOSITION;
               break;

            }
            switch ( ch )
            {

               case SP:
                  state = FIELD_VALUE_OPTIONAL_SP;
                  break;
               case HT:
                  state = FIELD_VALUE_OPTIONAL_HT;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case FIELD_VALUE_OPTIONAL_SP:
            if ( VCHAR( ch ) )
            {

               state = FIELD_VALUE_OPTIONAL_FIRST_AND_LAST_VCHAR_SUPERPOSITION;
               break;

            }
            switch ( ch )
            {

               case SP:
                  break;
               case HT:
                  state = FIELD_VALUE_OPTIONAL_HT;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case FIELD_VALUE_OPTIONAL_HT:
            if ( VCHAR( ch ) )
            {

               state = FIELD_VALUE_OPTIONAL_FIRST_AND_LAST_VCHAR_SUPERPOSITION;
               break;

            }
            switch ( ch )
            {

               case SP:
                  state = FIELD_VALUE_OPTIONAL_SP;
                  break;
               case HT:
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;
               
            }
            break;

         case FIELD_VALUE_OPTIONAL_FIRST_AND_LAST_VCHAR_SUPERPOSITION:
            if ( VCHAR( ch ) )
               break;
            switch ( ch )
            {

               /* OWS */
               case SP:
               case HT:
                  state = FIELD_LINE_SP_AND_TERMINATING_OWS_SUPERPOSITION;
                  break;
               case CR:
                  state = FIELD_LINE_CR;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case FIELD_LINE_SP_AND_TERMINATING_OWS_SUPERPOSITION:
            if ( VCHAR( ch ) )
            {

               state = FIELD_VALUE_OPTIONAL_FIRST_VCHAR;
               break;

            }
            switch ( ch )
            {

               /* OWS */
               case SP:
               case HT:
                  break;
               case CR:
                  state = FIELD_LINE_CR;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case FIELD_LINE_CR:
            
            if ( strt )
            {

               len = p - strt - 1;
               value = new u_char [ len + 1 ];
               if ( !value )
                  std::exit ( 1 );
               i = 0;
               for (; i < len ; i++ )
                  value [ i ] = strt [ i ];
               value [ i ] = '\0';
               insert ( r->headers , key , value );
               strt = NULL;
               
            }
            else
               delete key;
            switch ( ch )
            {

               case LF:
                  state = FIELD_LINE_LF;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case FIELD_LINE_LF:
            if ( TCHAR( ch ) )
            {

               strt = p;
               state = FIELD_NAME_TCHAR;
               break;

            }
            switch ( ch )
            {

               case CR:
                  state = CARRIAGE_RETURN;
                  break;
               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case CARRIAGE_RETURN:
            switch ( ch )
            {

               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return UNICORE_INVALID_FIELD_LINES_ERROR;

            }
            break;

         case LINE_FEED:
            b->pos = p;
            return UNICORE_VALID_FIELD_LINES_SUCCESS;

      }

   }

   return UNICORE_INVALID_FIELD_LINES_ERROR;

}
