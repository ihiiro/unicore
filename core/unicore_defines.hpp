

#pragma once


/* COMMON BYTES */
#ifndef LF
   #define LF 0x0A
#endif

#ifndef CR
   #define CR 0x0D
#endif

#ifndef SP
#define SP 0x20
#endif

#ifndef HT
   #define HT 0x09
#endif

#ifndef SERVER_SP
   #define SERVER_SP 0x25
#endif

/* STATUS CODE CATEGORIES */
#ifndef INFORMATIONAL
   #define INFORMATIONAL 0x31
#endif

#ifndef SUCCESSFUL
   #define SUCCESSFUL 0x32
#endif

#ifndef REDIRECTION
   #define REDIRECTION 0x33
#endif

#ifndef CLIENT_ERROR
   #define CLIENT_ERROR 0x34
#endif

#ifndef SERVER_ERROR
#define SERVER_ERROR 0x35
#endif

#ifndef VCHAR
   #define VCHAR( c ) ( c >= 0x21 and c <= 0x7E )
#endif
/* this PCHAR excludes pct-encoded */
/* PCHAR is defined in RFC 5234 or in URI's RFC  */
#ifndef PCHAR
   #define PCHAR( c ) ( c >= 0x41 and c <= 0x5A ) or ( c >= 0x61 and c <= 0x7A ) or \
                 ( c >= 0x30 and c <= 0x39 ) or c == 0x2D or c == 0x2E or \
                    c == 0x5F or c == 0x7E or c == 0x3A or c == 0x40 or \
                    c == 0x21 or c == 0x24 or ( c >= 0x26 and c <= 0x2C ) or \
                    c == 0x3B or c == 0x3D
#endif

#ifndef TCHAR
   #define TCHAR( c )  ( c >= 0x41 and c <= 0x5A ) or ( c >= 0x61 and c <= 0x7A ) or \
                 ( c >= 0x30 and c <= 0x39 ) or c == 0x21 or ( c >= 0x23 and c <= 0x27 ) or \
                 c == 0x2A or c == 0x2B or c == 0x2D or c == 0x2E or ( c >= 0x5E and c <= 0x60 ) or \
                 c == 0x7C or c == 0x7E 
#endif

/* bytes allowed in HOST, based on URI's RFC */
#ifndef HCHAR
   #define HCHAR( c ) ( c >= 0x41 and c <= 0x5A ) or ( c >= 0x61 and c <= 0x7A ) or \
                 ( c >= 0x30 and c <= 0x39 ) or c == 0x2D or c == 0x2E or \
                    c == 0x5F or c == 0x7E or c == 0x21 or c == 0x24 or \
                    ( c >= 0x26 and c <= 0x2C ) or c == 0x3B or c == 0x3D
#endif

#ifndef KiB
   #define KiB 1024
#endif

#ifndef MiB
   #define MiB 1048576
#endif

#ifndef GiB
   #define GiB 1073741824
#endif

#ifndef UNICORE_INVALID_REQUEST_LINE_ERROR
   #define UNICORE_INVALID_REQUEST_LINE_ERROR -1
#endif

#ifndef UNICORE_VALID_REQUEST_LINE_SUCCESS
   #define UNICORE_VALID_REQUEST_LINE_SUCCESS 1
#endif

#ifndef UNICORE_INCOMPLETE_REQUEST_LINE
   #define UNICORE_INCOMPLETE_REQUEST_LINE 2
#endif

#ifndef UNICORE_INVALID_FIELD_LINES_ERROR
   #define UNICORE_INVALID_FIELD_LINES_ERROR -1
#endif

#ifndef UNICORE_VALID_FIELD_LINES_SUCCESS
   #define UNICORE_VALID_FIELD_LINES_SUCCESS 1
#endif

#ifndef UNICORE_INCOMPLETE_FIELD_LINES
   #define UNICORE_INCOMPLETE_FIELD_LINES 2
#endif

#ifndef UNICORE_INVALID_CONFIG_FILE_ERROR
   #define UNICORE_INVALID_CONFIG_FILE_ERROR -1
#endif

#ifndef UNICORE_VALID_CONFIG_FILE_SUCCESS
   #define UNICORE_VALID_CONFIG_FILE_SUCCESS 1
#endif