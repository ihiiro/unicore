
#include "unicore_config_parse.hpp"
#include "unicore_defines.hpp"

#include <iostream>

int unicore_config_parse ( std::ifstream &s , unicore_config_t *c  )
{
    // (void)s;
    (void)c;
    int digit_count;
    char ch, error_pages[] = "ERROR_PAGES=", routes[] = "ROUTES=",
                mcms[] = "MAX_CLIENT_MESSAGE_SIZE=",server_name[] = "SERVER_NAME=";

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
       ROUTES,
       ROUTES_GET,
       ROUTES_POST,
       ROUTES_DELETE,
       ROUTES_SEPARATOR_AFTER_METHODS,
       ROUTES_ROOT,
       ROUTES_SEPARATOR_AFTER_ROOT,
       ROUTES_URL_FORWARD_SLASH,
       ROUTES_URL_SEGMENT,
       ROUTES_SEPARATOR_AFTER_URL,
       ROUTES_REDIRECTION_FORWARD_SLASH,
       ROUTES_REDIRECTION_SEGMENT,
       ROUTES_SEPARATOR_AFTER_REDIRECTION,
       ROUTES_UPLOAD_PATH,
       ROUTES_SEPARATOR_AFTER_UPLOAD_PATH,
       ROUTES_DIRECTORY_LISTING,
       ROUTES_SEPARATOR_AFTER_DIRECTORY_LISTING,
       ROUTES_FILE_IF_DIRECTORY_REQUEST,
       ROUTES_SEPARATOR_AFTER_FIDR,
       ROUTES_CGI_GET,
       ROUTES_CGI_POST,
       ROUTES_CGI_PYTHON,
       ROUTES_CGI_PHP,
       ROUTES_LF,
       ROUTES_HT,
       ROUTES_COMMENT,
       ERROR_PAGES,
       ERROR_PAGES_CODE,
       ERROR_PAGES_SEPARATOR_AFTER_CODE,
       ERROR_PAGES_SYSTEM_PATH,
       ERROR_PAGES_LF,
       ERROR_PAGES_HT,
       ERROR_PAGES_COMMENT,
       SERVER_NAME,
       SERVER_NAME_HOSTNAME,
       SERVER_NAME_LF,
       SERVER_NAME_HT,
       SERVER_NAME_COMMENT,
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
                    case 'M':
                        state = MCMS;
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
                if ( ch == '|' )
                    state = ROUTES_SEPARATOR_AFTER_ROOT;
                else if ( VCHAR( ch ) )
                    break;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_SEPARATOR_AFTER_ROOT:
                if ( ch == '/' )
                    state = ROUTES_URL_FORWARD_SLASH;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_URL_FORWARD_SLASH:
                if ( PCHAR( ch ) )
                {

                    state = ROUTES_URL_SEGMENT;
                    break;

                }
                switch ( ch )
                {

                    case '|':
                        state = ROUTES_SEPARATOR_AFTER_URL;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_URL_SEGMENT:
                if ( PCHAR( ch ) )
                    break;
                switch ( ch )
                {

                    case '|':
                        state = ROUTES_SEPARATOR_AFTER_URL;
                        break;
                    case '/':
                        state = ROUTES_URL_FORWARD_SLASH;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_SEPARATOR_AFTER_URL:
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

                    case '|':
                        state = ROUTES_SEPARATOR_AFTER_REDIRECTION;
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

                    case '/':
                        state = ROUTES_REDIRECTION_FORWARD_SLASH;
                        break;
                    case '|':
                        state = ROUTES_SEPARATOR_AFTER_REDIRECTION;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_SEPARATOR_AFTER_REDIRECTION:
                if ( VCHAR( ch ) )
                    state = ROUTES_UPLOAD_PATH;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_UPLOAD_PATH:
                if ( ch == '|' )
                    state = ROUTES_SEPARATOR_AFTER_UPLOAD_PATH;
                else if ( VCHAR( ch ) )
                    break;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_SEPARATOR_AFTER_UPLOAD_PATH:
                if ( ch == '0' or ch == '1' )
                    state = ROUTES_DIRECTORY_LISTING;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_DIRECTORY_LISTING:
                if ( ch == '|' )
                    state = ROUTES_SEPARATOR_AFTER_DIRECTORY_LISTING;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_SEPARATOR_AFTER_DIRECTORY_LISTING:
                if ( VCHAR( ch ) )
                    state = ROUTES_FILE_IF_DIRECTORY_REQUEST;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_FILE_IF_DIRECTORY_REQUEST:
                if ( ch == '|' )
                    state = ROUTES_SEPARATOR_AFTER_FIDR;
                else if ( VCHAR( ch ) )
                    break;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_SEPARATOR_AFTER_FIDR:
                if ( ch == '0' or ch == '1' )
                    state = ROUTES_CGI_GET;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_CGI_GET:
                if ( ch == '0' or ch == '1' )
                    state = ROUTES_CGI_POST;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_CGI_POST:
                if ( ch == '0' or ch == '1' )
                    state = ROUTES_CGI_PYTHON;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_CGI_PYTHON:
                if ( ch == '0' or ch == '1' )
                    state = ROUTES_CGI_PHP;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_CGI_PHP:
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
                    case 'E':
                        state = ERROR_PAGES;
                        break;
                    case 'M':
                        state = MCMS;
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
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

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
                if ( VCHAR( ch ) )
                    state = ERROR_PAGES_SYSTEM_PATH;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ERROR_PAGES_SYSTEM_PATH:
                if ( ch == '#' )
                    state = ERROR_PAGES_COMMENT;
                else if ( ch == LF )
                    state = ERROR_PAGES_LF;
                else if ( VCHAR( ch ) )
                    break;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
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
                if ( ch >= '0' and ch <= '9' )
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
                    case 'M':
                        state = MCMS;
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

                    case 'R':
                        state = ROUTES;
                        break;
                    case 'E':
                        state = ERROR_PAGES;
                        break;
                    case 'M':
                        state = MCMS;
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

                    case 'R':
                        state = ROUTES;
                        break;
                    case 'E':
                        state = ERROR_PAGES;
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


    if ( state == SERVER_BLOCK_TERMINAL_LF )
        return UNICORE_VALID_CONFIG_FILE_SUCCESS;
    return UNICORE_INVALID_CONFIG_FILE_ERROR;

}
