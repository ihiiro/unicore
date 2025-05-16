
#include "unicore_config_parse.hpp"
#include "unicore_defines.hpp"

#include <iostream>

int unicore_config_parse ( std::ifstream &s , unicore_config_t *c  )
{

    int digit_count = 0, i = 0, j = 0, len = 0;
    char ch = 0, error_pages[] = "ERROR_PAGES=", routes[] = "ROUTES=",
                mcms[] = "MAX_CLIENT_MESSAGE_SIZE=", primary_buf [ 512 ] , secondary_buf [ 512 ],
                *key, *value;
    unicore_route_t *route = NULL;
    std::memset ( primary_buf , 0 , 512 );
    std::memset ( secondary_buf , 0 , 512 );

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
       ROUTES_REDIRECTION_OLD_FORWARD_SLASH,
       ROUTES_REDIRECTION_OLD_SEGMENT,
       ROUTES_REDIRECTION_SEPARATOR_AFTER_OLD,
       ROUTES_REDIRECTION_NEW_FORWARD_SLASH,
       ROUTES_REDIRECTION_NEW_SEGMENT,
       ROUTES_REDIRECTION_LF,
       ROUTES_REDIRECTION_HT,
       ROUTES_COMMENT,
       ERROR_PAGES,
       ERROR_PAGES_CODE,
       ERROR_PAGES_SEPARATOR_AFTER_CODE,
       ERROR_PAGES_SYSTEM_PATH,
       ERROR_PAGES_LF,
       ERROR_PAGES_HT,
       ERROR_PAGES_COMMENT,
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

                    std::memset ( primary_buf , 0 , 512 );
                    i = 0;
                    primary_buf [ i++ ] = ch;
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
                {

                    if ( i > 510 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    primary_buf [ i++ ] = ch;
                    break;

                }

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

                    c [ j ].port = ch - '0';
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
                    {

                        c [ j ].port = c [ j ].port * 10 + ( ch - '0' );
                        digit_count++;

                    }
                    else
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    break;

                }
                else if ( ch == LF )
                {

                    if ( c [ j ].port >= 65535 or c [ j ].port <= 1024
                        or c [ j ].port == 49151 or c [ j ].port == 49152 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    // error if port already bound to another server
                    state = SERVER_BLOCK_LF;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case SERVER_BLOCK_LF:
                c [ j ].host = new char [ i + 1 ];
                for ( int k = 0 ; k < i ; k++ )
                    c [ j ].host [ k ] = primary_buf [ k ];
                c [ j ].host [ i ] = '\0';

                if ( ch == HT )
                    state = SERVER_BLOCK_HT;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case SERVER_BLOCK_HT:
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

                    std::memset ( primary_buf , 0 , 512 );
                    i = 0;
                    primary_buf [ i++ ] = ch;
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

                c [ j ].routes = new ht;
                c [ j ].routes->buckets = new bucket [ M ];
                std::memset ( c [ j ].routes->buckets , 0 , M );
                route = new unicore_route_t;

                if ( ch == '0' or ch == '1' )
                {

                    route->ROUTE_GET = ch - '0';
                    state = ROUTES_GET;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_GET:
                if ( ch == '0' or ch == '1' )
                {

                    route->ROUTE_POST = ch - '0';
                    state = ROUTES_POST;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_POST:
                if ( ch == '0' or ch == '1' )
                {

                    route->ROUTE_DELETE = ch - '0';
                    state = ROUTES_DELETE;

                }
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
                {

                    std::memset ( primary_buf , 0 , 512 );
                    i = 0;
                    primary_buf [ i++ ] = ch;
                    state = ROUTES_ROOT;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_ROOT:
                if ( ch == '|' )
                    state = ROUTES_SEPARATOR_AFTER_ROOT;
                else if ( VCHAR( ch ) )
                {

                    if ( i > 510 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    primary_buf [ i++ ] = ch;
                    break;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_SEPARATOR_AFTER_ROOT:
                for ( len = 0 ; primary_buf [ len ] ; len++ );
                route->root = new char [ len + 1 ];
                for ( int k = 0 ; k < len ; k++ )
                    route->root [ k ] = primary_buf [ k ];
                route->root [ i ] = '\0';
                if ( ch == '/' )
                {

                    std::memset ( primary_buf , 0 , 512 );
                    i = 0;
                    primary_buf [ i++ ] = ch;
                    state = ROUTES_URL_FORWARD_SLASH;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_URL_FORWARD_SLASH:
                if ( PCHAR( ch ) )
                {

                    if ( i > 510 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    primary_buf [ i++ ] = ch;
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
                {

                    if ( i > 510 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    primary_buf [ i++ ] = ch;
                    break;

                }
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
                if ( ch == '|' )
                {

                    route->ROUTE_UPLOADS = 0;
                    route->upload_path = NULL;
                    state = ROUTES_SEPARATOR_AFTER_UPLOAD_PATH;

                }
                else if ( VCHAR( ch ) )
                {

                    route->ROUTE_UPLOADS = 1;
                    std::memset ( secondary_buf , 0 , 512 );
                    i = 0;
                    secondary_buf [ i++ ] = ch;
                    state = ROUTES_UPLOAD_PATH;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_UPLOAD_PATH:
                if ( ch == '|' )
                {

                    for ( len = 0 ; secondary_buf [ len ] ; len++ );
                    route->upload_path = new char [ len + 1 ];
                    for ( int k = 0 ; k < len ; k++ )
                        route->upload_path [ k ] = secondary_buf [ k ];
                    route->upload_path [ i ] = '\0';
                    state = ROUTES_SEPARATOR_AFTER_UPLOAD_PATH;

                }
                else if ( VCHAR( ch ) )
                {

                    if ( i > 510 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    secondary_buf [ i++ ] = ch;
                    break;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_SEPARATOR_AFTER_UPLOAD_PATH:
                if ( ch == '0' or ch == '1' )
                {

                    route->directory_listing = ch - '0';
                    state = ROUTES_DIRECTORY_LISTING;

                }
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
                {

                    std::memset ( secondary_buf , 0 , 512 );
                    i = 0;
                    secondary_buf [ i++ ] = ch;
                    state = ROUTES_FILE_IF_DIRECTORY_REQUEST;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_FILE_IF_DIRECTORY_REQUEST:
                if ( ch == '|' )
                    state = ROUTES_SEPARATOR_AFTER_FIDR;
                else if ( VCHAR( ch ) )
                {

                    if ( i > 510 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    secondary_buf [ i++ ] = ch;
                    break;
                    
                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_SEPARATOR_AFTER_FIDR:
                for ( len = 0 ; secondary_buf [ len ] ; len++ );
                    route->file_if_directory_request = new char [ len + 1 ];
                for ( int k = 0 ; k < len ; k++ )
                    route->file_if_directory_request [ k ] = secondary_buf [ k ];
                route->file_if_directory_request [ i ] = '\0';
                if ( ch == '0' or ch == '1' )
                {

                    route->CGI_GET = ch - '0';
                    state = ROUTES_CGI_GET;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_CGI_GET:
                if ( ch == '0' or ch == '1' )
                {

                    route->CGI_POST = ch - '0';
                    state = ROUTES_CGI_POST;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_CGI_POST:
                if ( ch == '0' or ch == '1' )
                {

                    route->CGI_PYTHON = ch - '0';
                    state = ROUTES_CGI_PYTHON;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_CGI_PYTHON:
                if ( ch == '0' or ch == '1' )
                {

                    route->CGI_PHP = ch - '0';
                    state = ROUTES_CGI_PHP;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_CGI_PHP:
                key = new char [ i + 1 ];
                for ( int k = 0 ; k < i ; k++ )
                    key [ k ] = primary_buf [ k ];
                key [ i ] = '\0';
                insert ( c [ j ].routes , (u_char *)key , route );
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

                    route->ROUTE_GET = ch - '0';
                    state = ROUTES_GET;
                    break;

                }

                switch ( ch )
                {

                    case 'E':
                        state = ERROR_PAGES;
                        break;
                    case 'M':
                        state = MCMS;
                        break;
                    case '/':
                        c [ j ].redirection_list = new ht;
                        c [ j ].redirection_list->buckets = new bucket [ M ];
                        std::memset ( c [ j ].redirection_list->buckets , 0 , M );
                        std::memset ( primary_buf , 0 , 512 );
                        i = 0;
                        primary_buf [ i++ ] = ch;
                        state = ROUTES_REDIRECTION_OLD_FORWARD_SLASH;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_REDIRECTION_OLD_FORWARD_SLASH:
                if ( PCHAR( ch ) )
                {

                    if ( i > 510 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    primary_buf [ i++ ] = ch;
                    state = ROUTES_REDIRECTION_OLD_SEGMENT;

                }
                else if ( ch == '|' )
                    state = ROUTES_REDIRECTION_SEPARATOR_AFTER_OLD;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_REDIRECTION_OLD_SEGMENT:
                if ( PCHAR( ch ) )
                {

                    if ( i > 510 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    primary_buf [ i++ ] = ch;
                    break;

                }
                switch ( ch )
                {

                    case '/':
                        if ( i > 510 )
                            return UNICORE_INVALID_CONFIG_FILE_ERROR;
                        primary_buf [ i++ ] = ch;
                        state = ROUTES_REDIRECTION_OLD_FORWARD_SLASH;
                        break;
                    case '|':
                        state = ROUTES_REDIRECTION_SEPARATOR_AFTER_OLD;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_REDIRECTION_SEPARATOR_AFTER_OLD:
                if ( ch == '/' )
                {

                    std::memset ( secondary_buf , 0 , 512 );
                    i = 0;
                    secondary_buf [ i++ ] = ch;
                    state = ROUTES_REDIRECTION_NEW_FORWARD_SLASH;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_REDIRECTION_NEW_FORWARD_SLASH:
                if ( PCHAR( ch ) )
                {

                    if ( i > 510 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    secondary_buf [ i++ ] = ch;
                    state = ROUTES_REDIRECTION_NEW_SEGMENT;

                }
                else if ( ch == LF )
                    state = ROUTES_REDIRECTION_LF;
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ROUTES_REDIRECTION_NEW_SEGMENT:
                if ( PCHAR( ch ) )
                {

                    if ( i > 510 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    secondary_buf [ i++ ] = ch;
                    break;

                }
                switch ( ch )
                {

                    case LF:
                        state = ROUTES_REDIRECTION_LF;
                        break;
                    case '/':
                        if ( i > 510 )
                            return UNICORE_INVALID_CONFIG_FILE_ERROR;
                        secondary_buf [ i++ ] = ch;
                        state = ROUTES_REDIRECTION_NEW_FORWARD_SLASH;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_REDIRECTION_LF:
                // std::cout << "NEW URL [" << secondary_buf << "]\n";
                for ( len = 0 ; primary_buf [ len ] ; len++ );
                key = new char [ len + 1 ];
                for ( int k = 0 ; k < len ; k++ )
                    key [ k ] = primary_buf [ k ];
                key [ len ] = '\0';
                value = new char [ i + 1 ];
                for ( int k = 0 ; k < i ; k++ )
                    value [ k ] = secondary_buf [ k ];
                value [ i ] = '\0';
                insert ( c [ j ].redirection_list , (u_char *)key , value );
                switch ( ch )
                {

                    case LF:
                        state = SERVER_BLOCK_TERMINAL_LF;
                        break;
                    case HT:
                        state = ROUTES_REDIRECTION_HT;
                        break;
                    default:
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                }
                break;

            case ROUTES_REDIRECTION_HT:
                if ( ch == '/' )
                {

                    std::memset ( primary_buf , 0 , 512 );
                    i = 0;
                    primary_buf [ i++ ] = ch;
                    state = ROUTES_REDIRECTION_OLD_FORWARD_SLASH;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
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

                c [ j ].error_pages = new ht;
                c [ j ].error_pages->buckets = new bucket [ M ];
                std::memset ( c [ j ].error_pages->buckets , 0 , M );

                if ( ch >= '0' and ch <= '9' )
                {

                    std::memset ( primary_buf , 0 , 512 );
                    i = 0;
                    primary_buf [ i++ ] = ch;
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
                    {

                        if ( i > 510 )
                            return UNICORE_INVALID_CONFIG_FILE_ERROR;
                        primary_buf [ i++ ] = ch;
                        digit_count++;

                    }
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
                {

                    std::memset ( secondary_buf , 0 , 512 );
                    i = 0;
                    secondary_buf [ i++ ] = ch;
                    state = ERROR_PAGES_SYSTEM_PATH;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ERROR_PAGES_SYSTEM_PATH:
                if ( ch == '#' )
                    state = ERROR_PAGES_COMMENT;
                else if ( ch == LF )
                    state = ERROR_PAGES_LF;
                else if ( VCHAR( ch ) )
                {

                    if ( i > 510 )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    secondary_buf [ i++ ] = ch;
                    break;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case ERROR_PAGES_LF:
                key = new char [ i + 1 ];
                for ( int k = 0 ; k < i ; k++ )
                    key [ k ] = primary_buf [ k ];
                key [ i ] = '\0';
                value = new char [ i + 1 ];
                for ( int k = 0 ; k < i ; k++ )
                    value [ k ] = secondary_buf [ k ];
                value [ i ] = '\0';
                insert ( c [ j ].error_pages , (u_char *)key , value );
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

                    std::memset ( primary_buf , 0 , 512 );
                    i = 0;
                    primary_buf [ i++ ] = ch;
                    state = ERROR_PAGES_CODE;
                    digit_count = 1;
                    break;

                }
                switch ( ch )
                {

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

            case MCMS:
                for ( int i = 1 ; i < 24 ; i++, ch = s.get() )
                    if ( ch != mcms [ i ] )
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;

                if ( ch >= '0' and ch <= '9' )
                {

                    c [ j ].max_client_message_size = ch - '0';
                    state = MCMS_DIGIT;
                    digit_count = 1;

                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

            case MCMS_DIGIT:
                if ( ch >= '0' and ch <= '9' )
                {

                    if ( digit_count < 3 )
                    {

                        c [ j ].max_client_message_size = c [ j ].max_client_message_size * 10 + ( ch - '0' );
                        digit_count++;

                    }
                    else
                        return UNICORE_INVALID_CONFIG_FILE_ERROR;
                    break;

                }

                switch ( ch )
                {

                    /* normalize to bytes */
                    case 'B':
                        state = MCMS_B;
                        break;
                    case 'K':
                        c [ j ].max_client_message_size *= KiB;
                        state = MCMS_K;
                        break;
                    case 'M':
                        c [ j ].max_client_message_size *= MiB;
                        state = MCMS_M;
                        break;
                    case 'G':
                        c [ j ].max_client_message_size *= GiB;
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
                {

                    std::memset ( primary_buf , 0 , 512 );
                    i = 0;
                    primary_buf [ i++ ] = ch;
                    j++;
                    state = SERVER_BLOCK_SENTRY_HOST;


                }
                else
                    return UNICORE_INVALID_CONFIG_FILE_ERROR;
                break;

        }

    }


    if ( state == SERVER_BLOCK_TERMINAL_LF )
        return UNICORE_VALID_CONFIG_FILE_SUCCESS;
    return UNICORE_INVALID_CONFIG_FILE_ERROR;

}
