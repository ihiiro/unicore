

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

/* FUNCTION-LIKE MACROS */
#ifndef VCHAR
#define VCHAR( c ) ( c >= 0x21 and c <= 0x7E )
#endif
/* this PCHAR excludes sub-delims and pct-encoded */
/* PCHAR is defined in RFC 5234 or in URI's RFC  */
#ifndef PCHAR
#define PCHAR( c ) ( ( c >= 0x41 and c <= 0x5A ) or ( c >= 0x61 and c <= 0x7A ) or \
                 ( c >= 0x30 and c <= 0x39 ) or c == 0x2D or c == 0x2A or \
                    c == 0x5F or c == 0x7E or c == 0x3A c == 0x40 )
#endif

#ifndef UNICORE_INVALID_START_LINE_ERROR
#define UNICORE_INVALID_START_LINE_ERROR -1
#endif

#ifndef UNICORE_VALID_START_LINE_SUCCESS
#define UNICORE_VALID_START_LINE_SUCCESS 1
#endif
