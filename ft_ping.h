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
#include <getopt.h>
#include <limits.h>

#define F_VERBOSE   (1 << 0)
#define F_HELP      (1 << 1)
//-v -> verbose output
//-? / --help -> display help
//
#define RECV_TIMEOUT 1 //in seconds
#define PACKET_SIZE 56

typedef struct s_packet{
    struct icmphdr header;
    char msg[PACKET_SIZE];
} t_packet;

typedef struct s_ping{
    char *ip;
    char *hostname;
    int     sck;
    int     flags;
    short     ttl;
    long     count;
} t_ping;

struct s_timings{
    double min;
    double avg;
    double max;
    double mdev;
    double total;
    int count;
};

//parsing
void parse_args(int argc, char *argv[], t_ping* ctx);
// void set_flags(char* arg, int *flags);
char* gethost(char *argv);

//utils
void update_timings(struct s_timings *times, double pckt_msec);
void calc_endtimes(struct s_timings *times);
void show_usage(int);

#endif
