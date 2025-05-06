
#include "unicore_config_parse.hpp"

int unicore_config_parse ( std::ifstream &s , unicore_config_t *c  )
{
    char ch;

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
       

    } state;

    for ( ch = s.get() ; ch != traits_type::eof() ; ch = s.get() )
    {



    }

}
