
#include "unicore_config_parse.hpp"
#include "unicore_defines.hpp"

int unicore_config_parse ( std::ifstream &s , unicore_config_t *c  )
{
    // (void)s;
    (void)c;
    int digit_count;
    char ch, error_pages[] = "ERROR_PAGES=", uploads[] = "UPLOADS=",
                cgi[] = "COMMON_GATEWAY_INTERFACE=", fidr[] = "FILE_IF_DIRECTORY_REQUEST=",
                routes[] = "ROUTES=", mcms[] = "MAX_CLIENT_MESSAGE_SIZE=",
                server_name[] = "SERVER_NAME=";

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
                if ( HCHAR( ch ) )
                    break;

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
                    digit_count = 1;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case SERVER_BLOCK_SENTRY_PORT:
                if ( ch >= '0' and ch <= '9' )
                {

                    if ( digit_count < 5 )
                        digit_count++;
                    else
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    break;

                }
                else if ( ch == LF )
                {

                    // check if port is in unsigned 16bit range and > 1024,
                    //         // not 49151 and not 49152 and not 65535
                    //         // otherwise return error
                    state = SERVER_BLOCK_LF;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case SERVER_BLOCK_LF:
                if ( ch == HT )
                    state = SERVER_BLOCK_HT;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case SERVER_BLOCK_HT:
                switch ( ch )
                {

                    case 'S':
                        state = SERVER_NAME;
                        break;
                    case 'R':
                        state = ROUTES;
                        break;
                    case 'E':
                        state = ERROR_PAGES;
                        break;
                    case 'U':
                        state = UPLOADS;
                        break;
                    case 'C':
                        state = CGI;
                        break;
                    case 'M':
                        state = MCMS;
                        break;
                    case 'F':
                        state = FIDR;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case GLOBAL_COMMENT:
                if ( VCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case LF:
                        state = GLOBAL_LF;
                        break;
                    case HT:
                    case SP:
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case GLOBAL_LF:
                if ( HCHAR( ch ) )
                {

                    state = SERVER_BLOCK_SENTRY_HOST;
                    break;

                }
                switch ( ch )
                {

                    case LF:
                        break;
                    case '#':
                        state = GLOBAL_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case UPLOADS:
                for ( int i = 1 ; i < 8 ; i++, ch = s.get() )
                    if ( ch != uploads [ i ] )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                if ( ch == '/' )
                    state = UPLOADS_ROUTE_FORWARD_SLASH;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case UPLOADS_ROUTE_FORWARD_SLASH:
                if ( PCHAR( ch ) )
                    state = UPLOADS_SEGMENT;
                else if ( ch == '|' )
                    state = UPLOADS_SEPARATOR_AFTER_ROUTE;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case UPLOADS_SEGMENT:
                if ( PCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case '/':
                        state = UPLOADS_ROUTE_FORWARD_SLASH;
                        break;
                    case '|':
                        state = UPLOADS_SEPARATOR_AFTER_ROUTE;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case UPLOADS_SEPARATOR_AFTER_ROUTE:
                if ( VCHAR( ch ) )
                    state = UPLOADS_SYSTEM_PATH;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case UPLOADS_SYSTEM_PATH:
                if ( VCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case LF:
                        state = UPLOADS_LF;
                        break;
                    case '#':
                        state = UPLOADS_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case UPLOADS_LF:
                switch ( ch )
                {

                    case LF:
                        state = SERVER_BLOCK_TERMINAL_LF;
                        break;
                    case HT:
                        state = UPLOADS_HT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    
                }
                break;

            case UPLOADS_HT:
                switch ( ch )
                {

                    case 'S':
                        state = SERVER_NAME;
                        break;
                    case 'R':
                        state = ROUTES;
                        break;
                    case 'E':
                        state = ERROR_PAGES;
                        break;
                    case '/':
                        state = UPLOADS_ROUTE_FORWARD_SLASH;
                        break;
                    case 'C':
                        state = CGI;
                        break;
                    case 'M':
                        state = MCMS;
                        break;
                    case 'F':
                        state = FIDR;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case UPLOADS_COMMENT:
                if ( VCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case LF:
                        state = UPLOADS_LF;
                        break;
                    case HT:
                    case SP:
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;
            
            case ROUTES:
                for ( int i = 1 ; i < 7 ; i++, ch = s.get() )
                    if ( ch != routes [ i ] )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                if ( ch == '0' or ch == '1' )
                    state = ROUTES_GET;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;
            
            case ROUTES_GET:
                if ( ch == '0' or ch == '1' )
                    state = ROUTES_POST;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_POST:
                if ( ch == '0' or ch == '1' )
                    state = ROUTES_DELETE;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_DELETE:
                if ( ch == '|' )
                    state = ROUTES_SEPARATOR_AFTER_METHODS;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_SEPARATOR_AFTER_METHODS:
                if ( VCHAR( ch ) )
                    state = ROUTES_ROOT;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_ROOT:
                if ( VCHAR( ch ) )
                    break;
                else if ( ch == '|' )
                    state = ROUTES_SEPARATOR_AFTER_ROOT;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_SEPARATOR_AFTER_ROOT:
                if ( ch == '/' )
                    state = ROUTES_PATH_FORWARD_SLASH;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_PATH_FORWARD_SLASH:
                if ( PCHAR( ch ) )
                {

                    state = ROUTES_PATH_SEGMENT;
                    break;

                }
                switch ( ch )
                {

                    case LF:
                        state = ROUTES_LF;
                        break;
                    case '|':
                        state = ROUTES_SEPARATOR_AFTER_PATH;
                        break;
                    case '#':
                        state = ROUTES_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_PATH_SEGMENT:
                if ( PCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case LF:
                        state = ROUTES_LF;
                        break;
                    case '|':
                        state = ROUTES_SEPARATOR_AFTER_PATH;
                        break;
                    case '/':
                        state = ROUTES_PATH_FORWARD_SLASH;
                        break;
                    case '#':
                        state = ROUTES_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_SEPARATOR_AFTER_PATH:
                if ( ch == '/' )
                    state = ROUTES_REDIRECTION_FORWARD_SLASH;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_REDIRECTION_FORWARD_SLASH:
                if ( PCHAR( ch ) )
                {

                    state = ROUTES_REDIRECTION_SEGMENT;
                    break;

                }
                switch ( ch )
                {

                    case LF:
                        state = ROUTES_LF;
                        break;
                    case '#':
                        state = ROUTES_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_REDIRECTION_SEGMENT:
                if ( PCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case LF:
                        state = ROUTES_LF;
                        break;
                    case '/':
                        state = ROUTES_REDIRECTION_FORWARD_SLASH;
                        break;
                    case '#':
                        state = ROUTES_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_LF:
                if ( ch == HT )
                    state = ROUTES_HT;
                else if ( ch == LF )
                    state = SERVER_BLOCK_TERMINAL_LF;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;
            
            case ROUTES_HT:
                if ( ch == '0' or ch == '1' )
                {

                    state = ROUTES_GET;
                    break;

                }

                switch ( ch )
                {

                    case 'S':
                        state = SERVER_NAME;
                        break;
                    case 'U':
                        state = UPLOADS;
                        break;
                    case 'E':
                        state = ERROR_PAGES;
                        break;
                    case 'C':
                        state = CGI;
                        break;
                    case 'M':
                        state = MCMS;
                        break;
                    case 'F':
                        state = FIDR;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_COMMENT:
                if ( VCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case LF:
                        state = ROUTES_LF;
                        break;
                    case HT:
                    case SP:
                        break;

                }
                break;

            case ERROR_PAGES:
                for ( int i = 1 ; i < 12 ; i++, ch = s.get() )
                    if ( ch != error_pages [ i ] )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                if ( ch >= '0' and ch <= '9' )
                {

                    state = ERROR_PAGES_CODE;
                    digit_count = 1;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ERROR_PAGES_CODE:
                if ( ch >= '0' and ch <= '9' )
                {

                    if ( digit_count < 3 )
                        digit_count++;
                    else
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    break;

                }
                else if ( ch == '|' )
                    state = ERROR_PAGES_SEPARATOR_AFTER_CODE;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ERROR_PAGES_SEPARATOR_AFTER_CODE:
                if ( ch == '/' )
                    state = ERROR_PAGES_PATH_FORWARD_SLASH;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ERROR_PAGES_PATH_FORWARD_SLASH:
                if ( PCHAR( ch ) )
                {

                    state = ERROR_PAGES_SEGMENT;
                    break;

                }
                switch ( ch )
                {

                    case LF:
                        state = ERROR_PAGES_LF;
                        break;
                    case '#':
                        state = ERROR_PAGES_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ERROR_PAGES_SEGMENT:
                if ( PCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case LF:
                        state = ERROR_PAGES_LF;
                        break;
                    case '/':
                        state = ERROR_PAGES_PATH_FORWARD_SLASH;
                        break;
                    case '#':
                        state = ERROR_PAGES_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ERROR_PAGES_LF:
                switch ( ch )
                {

                    case LF:
                        state = SERVER_BLOCK_TERMINAL_LF;
                        break;
                    case HT:
                        state = ERROR_PAGES_HT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ERROR_PAGES_HT:
                if ( ch == '0' or ch == '1' )
                {

                    state = ERROR_PAGES_CODE;
                    digit_count = 1;
                    break;

                }
                switch ( ch )
                {

                    case 'S':
                        state = SERVER_NAME;
                        break;
                    case 'R':
                        state = ROUTES;
                        break;
                    case 'U':
                        state = UPLOADS;
                        break;
                    case 'C':
                        state = CGI;
                        break;
                    case 'M':
                        state = MCMS;
                        break;
                    case 'F':
                        state = FIDR;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ERROR_PAGES_COMMENT:
                if ( VCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case LF:
                        state = ERROR_PAGES_LF;
                        break;
                    case HT:
                    case SP:
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case CGI:
                for ( int i = 1 ; i < 25 ; i++, ch = s.get() )
                    if ( ch != cgi [ i ] )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                if ( ch == '0' or ch == '1' )
                    state = CGI_GET;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case CGI_GET:
                if ( ch == '0' or ch == '1' )
                    state = CGI_POST;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case CGI_POST:
                if ( ch == '|' )
                    state = CGI_SEPARATOR_AFTER_METHODS;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case CGI_SEPARATOR_AFTER_METHODS:
                if ( ch == '0' or ch == '1' )
                    state = CGI_PYTHON;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case CGI_PYTHON:
                if ( ch == '0' or ch == '1' )
                    state = CGI_PHP;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case CGI_PHP:
                switch ( ch )
                {

                    case LF:
                        state = CGI_LF;
                        break;
                    case '#':
                        state = CGI_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case CGI_LF:
                switch ( ch )
                {

                    case LF:
                        state = SERVER_BLOCK_TERMINAL_LF;
                        break;
                    case HT:
                        state = CGI_HT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }

            case CGI_HT:
                if ( ch == '0' or ch == '1' )
                {

                    state = CGI_GET;
                    break;

                }

                switch ( ch )
                {

                    case 'S':
                        state = SERVER_NAME;
                        break;
                    case 'R':
                        state = ROUTES;
                        break;
                    case 'E':
                        state = ERROR_PAGES;
                        break;
                    case 'U':
                        state = UPLOADS;
                        break;
                    case 'M':
                        state = MCMS;
                        break;
                    case 'F':
                        state = FIDR;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case CGI_COMMENT:
                if ( VCHAR( ch ) )
                    break;

                switch ( ch )
                {

                    case LF:
                        state = CGI_LF;
                        break;
                    case HT:
                    case SP:
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case SERVER_NAME:
                for ( int i = 1 ; i < 12 ; i++, ch = s.get() )
                    if ( ch != server_name [ i ] )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                if ( HCHAR( ch ) )
                    state = SERVER_NAME_HOSTNAME;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case SERVER_NAME_HOSTNAME:
                if ( HCHAR( ch ) )
                    break;

                switch ( ch )
                {

                    case LF:
                        state = SERVER_NAME_LF;
                        break;
                    case '#':
                        state = SERVER_NAME_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case SERVER_NAME_LF:
                switch ( ch )
                {

                    case LF:
                        state = SERVER_BLOCK_TERMINAL_LF;
                        break;
                    case HT:
                        state = SERVER_NAME_HT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case SERVER_NAME_HT:
                switch ( ch )
                {

                    case 'C':
                        state = CGI;
                        break;
                    case 'R':
                        state = ROUTES;
                        break;
                    case 'E':
                        state = ERROR_PAGES;
                        break;
                    case 'U':
                        state = UPLOADS;
                        break;
                    case 'M':
                        state = MCMS;
                        break;
                    case 'F':
                        state = FIDR;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case SERVER_NAME_COMMENT:
                if ( VCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case LF:
                        state = SERVER_NAME_LF;
                        break;
                    case HT:
                    case SP:
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case FIDR:
                for ( int i = 1 ; i < 26 ; i++, ch = s.get() )
                    if ( ch != fidr [ i ] )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                if ( VCHAR( ch ) )
                    state = FIDR_SYSTEM_PATH;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case FIDR_SYSTEM_PATH:
                if ( VCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case LF:
                        state = FIDR_LF;
                        break;
                    case '#':
                        state = FIDR_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case FIDR_LF:
                switch ( ch )
                {

                    case LF:
                        state = SERVER_BLOCK_TERMINAL_LF;
                        break;
                    case HT:
                        state = FIDR_HT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case FIDR_HT:
                switch ( ch )
                {

                    case 'C':
                        state = CGI;
                        break;
                    case 'R':
                        state = ROUTES;
                        break;
                    case 'E':
                        state = ERROR_PAGES;
                        break;
                    case 'U':
                        state = UPLOADS;
                        break;
                    case 'M':
                        state = MCMS;
                        break;
                    case 'S':
                        state = SERVER_NAME;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case FIDR_COMMENT:
                if ( VCHAR( ch ) )
                    break;

                switch ( ch )
                {

                    case LF:
                        state = FIDR_LF;
                        break;
                    case HT:
                    case SP:
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case MCMS:
                for ( int i = 1 ; i < 24 ; i++, ch = s.get() )
                    if ( ch != mcms [ i ] )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                if ( ch >= '0' and ch <= '9' )
                {

                    state = MCMS_DIGIT;
                    digit_count = 1;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case MCMS_DIGIT:
                if ( ch >= '0' and ch <= '9' )
                {

                    if ( digit_count < 10 )
                        digit_count++;
                    else
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    break;

                }

                switch ( ch )
                {

                    case 'B':
                        state = MCMS_B;
                        break;
                    case 'K':
                        state = MCMS_K;
                        break;
                    case 'M':
                        state = MCMS_M;
                        break;
                    case 'G':
                        state = MCMS_G;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case MCMS_B:
            case MCMS_K:
            case MCMS_M:
            case MCMS_G:
                switch ( ch )
                {

                    case LF:
                        state = MCMS_LF;
                        break;
                    case '#':
                        state = MCMS_COMMENT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case MCMS_LF:
                switch ( ch )
                {

                    case LF:
                        state = SERVER_BLOCK_TERMINAL_LF;
                        break;
                    case HT:
                        state = MCMS_HT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case MCMS_HT:
                switch ( ch )
                {

                    case 'C':
                        state = CGI;
                        break;
                    case 'R':
                        state = ROUTES;
                        break;
                    case 'E':
                        state = ERROR_PAGES;
                        break;
                    case 'U':
                        state = UPLOADS;
                        break;
                    case 'F':
                        state = FIDR;
                        break;
                    case 'S':
                        state = SERVER_NAME;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case MCMS_COMMENT:
                if ( VCHAR( ch ) )
                    break;

                switch ( ch )
                {

                    case LF:
                        state = MCMS_LF;
                        break;
                    case HT:
                    case SP:
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case SERVER_BLOCK_TERMINAL_LF:
                if ( HCHAR( ch ) )
                    state = SERVER_BLOCK_SENTRY_HOST;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

        }

    }

    return UNICORE_INVALID_CONFIG_FILE_ERROR;

}
