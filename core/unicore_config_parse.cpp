
#include "unicore_config_parse.hpp"
#include "unicore_defines.hpp"

int unicore_config_parse ( std::ifstream &s , unicore_config_t *c  )
{
    // (void)s;
    (void)c;
    char ch;
    int port_digit_count;

    enum
    {

       START = 0,
       SERVER_BLOCK_SENTRY_HOST,
       SERVER_BLOCK_SENTRY_COLON,
       SERVER_BLOCK_SENTRY_PORT,
       SERVER_BLOCK_LF,
       SERVER_BLOCK_HT,
       GLOBAL_COMMENT,
       GLOBAL_LF,
       UPLOADS,
       UPLOADS_ROUTE_FORWARD_SLASH,
       UPLOADS_SEGMENT,
       UPLOADS_SEPARATOR_AFTER_ROUTE,
       UPLOADS_SYSTEM_PATH,
       UPLOADS_LF,
       UPLOADS_HT,
       UPLOADS_COMMENT,
       ROUTES,
       ROUTES_GET,
       ROUTES_POST,
       ROUTES_DELETE,
       ROUTES_SEPARATOR_AFTER_METHODS,
       ROUTES_ROOT,
       ROUTES_SEPARATOR_AFTER_ROOT,
       ROUTES_PATH_FORWARD_SLASH,
       ROUTES_PATH_SEGMENT,
       ROUTES_SEPARATOR_AFTER_PATH,
       ROUTES_REDIRECTION_FORWARD_SLASH,
       ROUTES_REDIRECTION_SEGMENT,
       ROUTES_LF,
       ROUTES_HT,
       ROUTES_COMMENT,
       ERROR_PAGES,
       ERROR_PAGES_CODE,
       ERROR_PAGES_SEPARATOR_AFTER_CODE,
       ERROR_PAGES_PATH_FORWARD_SLASH,
       ERROR_PAGES_SEGMENT,
       ERROR_PAGES_LF,
       ERROR_PAGES_HT,
       ERROR_PAGES_COMMENT,
       CGI,
       CGI_GET,
       CGI_POST,
       CGI_SEPARATOR_AFTER_METHODS,
       CGI_PYTHON,
       CGI_PHP,
       CGI_LF,
       CGI_HT,
       CGI_COMMENT,
       SERVER_NAME,
       SERVER_NAME_HOSTNAME,
       SERVER_NAME_LF,
       SERVER_NAME_HT,
       SERVER_NAME_COMMENT,
       FIDR, /* FILE_IF_DIRECTORY_REQUEST */
       FIDR_SYSTEM_PATH,
       FIDR_LF,
       FIDR_HT,
       FIDR_COMMENT,
       MCMS, /* MAX_CLIENT_MESSAGE_SIZE */
       MCMS_DIGIT,
       MCMS_B,
       MCMS_K,
       MCMS_M,
       MCMS_G,
       MCMS_LF,
       MCMS_HT,
       MCMS_COMMENT,
       SERVER_BLOCK_TERMINAL_LF

    } state;

    state = START;
    for ( ch = s.get() ; ch != traits_type::eof() ; ch = s.get() )
    {

        switch ( state )
        {

            case START:
                if ( HCHAR( ch ) )
                {

                    state = SERVER_BLOCK_SENTRY_HOST;
                    break;

                }
                switch ( ch )
                {

                    case LF:
                        state = GLOBAL_LF;
                        break;
                    case '#':
                        state = GLOBAL_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case SERVER_BLOCK_SENTRY_HOST:
                switch ( ch )
                {

                    case LF:
                        state = SERVER_BLOCK_LF;
                        break;
                    case ':':
                        state = SERVER_BLOCK_SENTRY_COLON;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case SERVER_BLOCK_SENTRY_COLON:
                if ( ch >= '0' and ch <= '9' )
                {

                    state = SERVER_BLOCK_SENTRY_PORT;
                    port_digit_count = 1;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case SERVER_BLOCK_SENTRY_PORT:
                if ( ch >= '0' and ch <= '9' )
                {

                    if ( port_digit_count < 5 )
                        port_digit_count++;
                    else
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    break;

                }
                switch ( ch )
                {

                    case LF:
                        // check if port is in unsigned 16bit range and > 1024,
                        // not 49151 and not 49152 and not 65535
                        // otherwise return error
                        state = SERVER_BLOCK_LF;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

        }

    }

    return UNICORE_INVALID_CONFIG_FILE_ERROR;

}
