

#pragma once

#include <sys/types.h>

typedef struct
{
    u_int           method;
    u_int           http_version;
    u_int             state;          /* FSM's most recent state */
} unicore_request_t;