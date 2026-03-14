#include "../ft_ping.h"

void update_timings(struct s_timings *times, double pckt_msec){
    if (pckt_msec > times->max)
        times->max = pckt_msec;

    if (pckt_msec < times->min || times->min < 0.001)
        times->min = pckt_msec;

    times->total += pckt_msec;
    times->count++;
}

void calc_endtimes(struct s_timings *times){
    times->avg = times->total / times->count;

    times->mdev = 0; //TODO
}

void show_usage(int extval){
    printf("Usage: sudo ./ft_ping [-v] [-?] <hostname>\n");
    printf("  -v, --verbose         Show extra error information (e.g., unreachable hosts)\n");
    // printf("  -c, --count COUNT     Stop after sending <count> packets\n");
    // printf("  -i, --interval SECS   Interval between packets\n");
    // printf("  -s, --size SIZE       Payload size in bytes\n");
    // printf("  -q, --quiet           Suppress output except for errors\n");
    printf("  -?, --help            Show this help message\n");
    // printf("  -l, --preload PRELOAD Number of packets to preload before normal behavior\n");
    printf("  -h, --help            Show this help message\n");
    exit(extval);
}
