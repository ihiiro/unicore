

#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <cstring>

/* HASH TABLE */
#ifndef FNV_PRIME
 #define FNV_PRIME 1099511628211UL
#endif

#ifndef FNV_OFFSET_BASIS
 #define FNV_OFFSET_BASIS 14695981039346656037UL
#endif

#ifndef PROBE_LENGTH
 #define PROBE_LENGTH 1
#endif

#ifndef M
 #define M 200
#endif

typedef struct
{
    const u_char *key;
    const u_char *value;
} bucket;

typedef struct 
{
    bucket *buckets;
} ht;

uint64_t FNV_1A_HASH ( const u_char *k );
uint64_t hash ( const u_char *k );
void insert ( ht *HT , const u_char *key , const u_char *value );
bucket *get ( ht *HT , const u_char *key );

typedef struct
{

    u_int           method;
    u_int           http_version;

    ht              *headers; /* hash table by field name */

} unicore_request_t;