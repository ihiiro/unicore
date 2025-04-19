

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
        CR_OR_LF,
        CRLF_CONSUMED,
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
               case ' ':
                  state = START_LINE_PRECEDING_SP_FIELD;
                  break;
               case CR:
               case LF:
                  state = CR_OR_LF;
                  break;

            }
            break;

      }

   }

}