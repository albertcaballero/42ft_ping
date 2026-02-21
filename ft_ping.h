#ifndef FT_PING_H_
#define FT_PING_H_

#include "libft/libft.h"
#include <sys/socket.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define F_VERBOSE   (1 << 0)
#define F_HELP      (1 << 1)
//-v -> verbose output
//-? / --help -> display help
//
#define RECV_TIMEOUT 1 //in seconds
#define PACKET_SIZE 64

typedef struct s_ping
{
    char *ip;
    char *header;
    struct timeval time;
    char* padding;
} t_ping;

typedef struct s_packet{
    struct icmphdr header;
    char msg[PACKET_SIZE];
} t_packet;

typedef struct s_program{
    char *ip;
    char *hostname;
    char *revdns;
    struct sockaddr addr;
} t_program;

//parsing
void set_flags(char* arg, int *flags);
char *clean_args(char **argv, int argc);



#endif
