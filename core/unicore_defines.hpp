

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


#ifndef VCHAR
#define VCHAR( c ) ( c >= 0x21 && c <= 0x7E )
#endif


#ifndef UNICORE_INVALID_START_LINE_ERROR
#define UNICORE_INVALID_START_LINE_ERROR -1
#endif

#ifndef UNICORE_VALID_START_LINE_SUCCESS
#define UNICORE_VALID_START_LINE_SUCCESS 1
#endif
