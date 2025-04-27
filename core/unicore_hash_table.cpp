
#include "unicore_request.hpp"

uint64_t FNV_1A_HASH ( const u_char *k )
{

    uint64_t hash = FNV_OFFSET_BASIS;
    for ( ; *k ; k++ )
    {

        hash ^= *k;
        hash *= FNV_PRIME;

    }

    return hash;

}

uint64_t hash ( const u_char *k )
{

    return FNV_1A_HASH ( k ) % M ;

}

void insert ( ht *HT , const u_char *key , const u_char *value )
{

    uint64_t index = hash ( key );

    /* linear probing with a fixed length */
    while ( 1 )
    {

        if ( index == M )
        {

            index = 0;
            continue;

        }
        if ( HT->buckets [ index ].key == NULL )
        {

            HT->buckets [ index ].key = key;
            HT->buckets [ index ].value = value;
            return;

        }

        index += PROBE_LENGTH;

    }

}

#include <iostream>

bucket *get ( ht *HT , const u_char *key )
{

    uint64_t index = hash ( key );
    bool stop = 0;

    while ( 1 )
    {
        if ( index == M )
        {

            if ( stop )
                return NULL;
            stop = 1;
            index = 0;
            continue;

        }
        if ( key and HT->buckets [ index ].key and 
            std::strcmp ( (char *)key , (char *)HT->buckets [ index ].key  ) == 0 )
            return &HT->buckets [ index ];

        index += PROBE_LENGTH;

    }

}
