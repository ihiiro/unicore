

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


#define UNICORE_INVALID_START_LINE_ERROR -1