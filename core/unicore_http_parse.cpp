

#include "unicore_buf.hpp"
#include "unicore_request.hpp"
#include "unicore_status.hpp"
#include "unicore_defines.hpp"

#include <sys/types.h>


int    
 unicore_http_parse_start_line ( unicore_request_t *r , unicore_buf_t *b , unicore_status_t *s )
{

   u_char ch, *p;

   enum state
   {

        START = 0,
        START_CR,
        START_LF,
        START_LINE_PRECEDING_SP_FIELD,
        STATUS_LINE_START,
        STATUS_LINE_HTTP_T1_ALPHA,
        STATUS_LINE_HTTP_T2_ALPHA,
        STATUS_LINE_HTTP_P_ALPHA,
        STATUS_LINE_HTTP_FORWARD_SLASH_BEFORE_VERSION,
        STATUS_LINE_HTTP_MAJOR_VERSION_1,
        STATUS_LINE_HTTP_VERSION_DOT,
        STATUS_LINE_HTTP_MINOR_VERSION_1,
        STATUS_LINE_HTTP_VERSION_VALIDATED_BY_SP,
        INFO_STATUS_CODE_START,         /* code 1## */
        SUCCESSFUL_STATUS_CODE_START,   /* code 2## */
        REDICRECTION_STATUS_CODE_START, /* code 3## */
        CLIENT_ERROR_STATUS_CODE_START, /* code 4## */
        SERVER_ERROR_STATUS_CODE_START, /* code 5## */
        STATUS_CODE_SECOND_DIGIT,
        STATUS_CODE_THIRD_DIGIT,
        STATUS_CODE_VALIDATED_BY_SP,
        CARRIAGE_RETURN,
        LINE_FEED,
        REASON_PHRASE,
        REQUEST_LINE_START,
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
        REQUST_LINE_HTTP_H_ALPHA,
        REQUEST_LINE_HTTP_T1_ALPHA,
        REQUEST_LINE_HTTP_T2_ALPHA,
        REQUEST_LINE_HTTP_P_ALPHA,
        REQUEST_LINE_HTTP_FORWARD_SLASH_BEFORE_VERSION,
        REQUEST_LINE_HTTP_MAJOR_VERSION_1,
        REQUEST_LINE_HTTP_VERSION_DOT,
        REQUEST_LINE_HTTP_MINOR_VERSION_1,
        REQUEST_LINE_TRAILING_SP_FIELD

   } state;

   state = static_cast<enum state>(r->state);
   for ( p = b->pos; p < b->end ; p++ )
   {

      ch = *p;

      switch ( state )
      {
      
         /* first start-line character */
         case START:
            switch ( ch )
            {

               case 'H':
                  state = STATUS_LINE_START;
                  break;
               case 'G':
               case 'P':
               case 'D':
                  state = REQUEST_LINE_START;
                  break;
               case SP:
                  state = START_LINE_PRECEDING_SP_FIELD;
                  break;
               case CR:
                  state = START_CR;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;
         
         case START_CR:
            switch ( ch )
            {

               case LF:
                  state = START_LF;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;
               

            }
            break;

         case START_LF:
            switch ( ch )
            {

               case 'G':
               case 'P':
               case 'D':
                  state = REQUEST_LINE_START;
                  break;
               case CR:
                  state = START_CR;
                  break;
               case SP:
                  state = START_LINE_PRECEDING_SP_FIELD;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;
               
            }
            break;

         case START_LINE_PRECEDING_SP_FIELD:
            switch ( ch )
            {

               case 'H':
                  state = STATUS_LINE_START;
                  break;
               case 'G':
               case 'P':
               case 'D':
                  state = REQUEST_LINE_START;
                  break;
               case SP:
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;

         case STATUS_LINE_START:
            switch ( ch )
            {

               case 'T':
                  state = STATUS_LINE_HTTP_T1_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;

         case STATUS_LINE_HTTP_T1_ALPHA:
            switch ( ch )
            {

               case 'T':
                  state = STATUS_LINE_HTTP_T2_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }

         case STATUS_LINE_HTTP_T2_ALPHA:
            switch ( ch )
            {

               case 'P':
                  state = STATUS_LINE_HTTP_P_ALPHA;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }

         case STATUS_LINE_HTTP_P_ALPHA:
            switch ( ch )
            {

               case '/':
                  state = STATUS_LINE_HTTP_FORWARD_SLASH_BEFORE_VERSION;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;

         case STATUS_LINE_HTTP_FORWARD_SLASH_BEFORE_VERSION:
            switch ( ch )
            {

               case '1':
                  state = STATUS_LINE_HTTP_MAJOR_VERSION_1;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;

         case STATUS_LINE_HTTP_MAJOR_VERSION_1:
            switch ( ch )
            {

               case '.':
                  state = STATUS_LINE_HTTP_VERSION_DOT;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;

         case STATUS_LINE_HTTP_VERSION_DOT:
            switch ( ch )
            {

               case '1':
                  state = STATUS_LINE_HTTP_MINOR_VERSION_1;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;

         case STATUS_LINE_HTTP_MINOR_VERSION_1:
            switch ( ch )
            {

               case SP:
                  state  = STATUS_LINE_HTTP_VERSION_VALIDATED_BY_SP;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;

         case STATUS_LINE_HTTP_VERSION_VALIDATED_BY_SP:
            switch ( ch )
            {

               case SP:
                  break;
               case INFORMATIONAL:
                  state = INFO_STATUS_CODE_START;
                  break;
               case SUCCESSFUL:
                  state = SUCCESSFUL_STATUS_CODE_START;
                  break;
               case REDIRECTION:
                  state = REDICRECTION_STATUS_CODE_START;
                  break;
               case CLIENT_ERROR:
                  state = CLIENT_ERROR_STATUS_CODE_START;
                  break;
               case SERVER_ERROR:
                  state = SERVER_ERROR_STATUS_CODE_START;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;

         case INFO_STATUS_CODE_START:
         case SUCCESSFUL_STATUS_CODE_START:
         case REDICRECTION_STATUS_CODE_START:
         case CLIENT_ERROR_STATUS_CODE_START:
         case SERVER_ERROR_STATUS_CODE_START:
            if ( ch >= '0' and ch <= '9' )
               state = STATUS_CODE_SECOND_DIGIT;
            else
               return UNICORE_INVALID_START_LINE_ERROR;
            break;

         case STATUS_CODE_SECOND_DIGIT:
            if ( ch >= '0' and ch <= '9' )
               state = STATUS_CODE_THIRD_DIGIT;
            else
               return UNICORE_INVALID_START_LINE_ERROR;
            break;

         case STATUS_CODE_THIRD_DIGIT:
            switch ( ch )
            {

               case SP:
                  state = STATUS_CODE_VALIDATED_BY_SP;
                  break;
               case CR:
                  state = CARRIAGE_RETURN;
                  break;
               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;

         case STATUS_CODE_VALIDATED_BY_SP:
/* TODO: support for SP in reason-phrase if required */
            if ( VCHAR(ch) or ch == HT ) 
            {

               state = REASON_PHRASE;
               break;

            }

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
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;

         case CARRIAGE_RETURN:
            switch ( ch )
            {

               case LF:
                  state = LINE_FEED;
                  break;
               default:
                  return UNICORE_INVALID_START_LINE_ERROR;

            }
            break;

         case LINE_FEED:
            return UNICORE_VALID_START_LINE_SUCCESS;

         

         

      }

   }

}