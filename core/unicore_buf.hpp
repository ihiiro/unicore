
#pragma once
#include <sys/types.h>

typedef struct
{ 
    u_char          *pos;
    u_char          *start; /* start of buffer */
    u_char          *end;   /* end of buffer */
} unicore_buf_t;