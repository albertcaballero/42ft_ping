#ifndef FT_PING_H_
#define FT_PING_H_

#include "libft/libft.h"

#define F_VERBOSE   (1 << 0)
#define F_HELP      (1 << 1)
//-v -> verbose output
//-? / --help -> display help

typedef struct s_ping
{
    char *ip;
    char *header;
    struct timeval time;
    char* padding;
} t_ping;

#endif
