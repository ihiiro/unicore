

#pragma once
#include <fstream>

typedef std::ifstream::traits_type traits_type;



typedef struct
{
    /* stuff */
} unicore_config_t;

int unicore_config_parse ( std::ifstream &s , unicore_config_t *c  );