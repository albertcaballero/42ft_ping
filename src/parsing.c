#include "../ft_ping.h"
#include <bits/getopt_core.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

char* gethost(char *argv){
    struct addrinfo hints;
    struct addrinfo *results;
    char ip[INET6_ADDRSTRLEN];

    ft_bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int rd = getaddrinfo(argv, NULL, &hints, &results);
    if (rd != 0){
        ft_dprintf(2, "ft_ping: %s: %s\n", argv, gai_strerror(rd));
        return NULL;
    }

    for (struct addrinfo *p = results; p != NULL; p = p->ai_next) {
        void *addr;

        if (p->ai_family == AF_INET) {
            addr = &((struct sockaddr_in *)p->ai_addr)->sin_addr;
        } else {
            addr = &((struct sockaddr_in6 *)p->ai_addr)->sin6_addr;
        }

        inet_ntop(p->ai_family, addr, ip, sizeof(ip));
        // printf("IP: %s\n", ip);
        break;
    }
    freeaddrinfo(results);
    return ft_strdup(ip);
}

void parse_ttl(char *optarg, t_ping* ctx){
    int err = 0;
    err = ft_strlen(optarg) > 3;

    int ttl = ft_atoi(optarg);
    err += ttl > 255 || ttl < 0;

    if (err){
        ft_dprintf(STDERR_FILENO, "ft_ping: invalid argument: '%s': out of range: 0 <= value <= 255\n", optarg);
        exit(EXIT_FAILURE);
    }

    ctx->ttl = ttl;
}

void parse_count(char *optarg, t_ping* ctx){
    int err = 0;
    err = ft_strlen(optarg) > 19;

    long count = ft_atol(optarg);
    err += count <= 0 || count > LONG_MAX;

    if (err){
        ft_dprintf(STDERR_FILENO, "ft_ping: invalid argument: '%s': out of range: 1 <= value <= 9223372036854775807\n", optarg);
        exit(EXIT_FAILURE);
    }
    ctx->count = count;
}

void parse_linger(char *optarg, t_ping* ctx){
    int err = 0;
    err = ft_strlen(optarg) > 6;

    long linger = ft_atoi(optarg);
    err += linger <= 0 || linger > 999999;

    if (err){
        ft_dprintf(STDERR_FILENO, "ft_ping: bad linger time: '%s': out of range: 1 <= value <= 999999\n", optarg);
        exit(EXIT_FAILURE);
    }
    ctx->linger = linger;
}

void parse_preload(char *optarg, t_ping* ctx){
    int err = 0;
    err = ft_strlen(optarg) > 2;

    long load = ft_atoi(optarg);
    err += load <= 0 || load > 3;

    if (err){
        ft_dprintf(STDERR_FILENO, "ft_ping: cannot set preload value greater than 3: %s\n", optarg);
        exit(EXIT_FAILURE);
    }
    ctx->preload = load;
}

void parse_interval(char *optarg, t_ping* ctx){
    int err = 0;
    err = ft_strlen(optarg) > 6;

    long val = ft_atoi(optarg);
    err += val <= 0 || val > 999999;

    if (err){
        ft_dprintf(STDERR_FILENO, "ft_ping: bad timing interval: '%s': out of range: 1 <= value <= 999999\n", optarg);
        exit(EXIT_FAILURE);
    }
    ctx->itvl = val;
}

void parse_args(int argc, char *argv[], t_ping* ctx)
{
    int         opt;

    ctx->count = LONG_MAX;
    ctx->ttl = 64;
    ctx->preload = -1;
    ctx->linger = 1;
    ctx->itvl = 1;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'},
        {"ttl", required_argument, 0, 0},
        {"count", required_argument, 0, 'c'},
        {"flood", no_argument, 0, 'f'},
        {"linger", required_argument, 0, 'W'},
        {"preload", required_argument, 0, 'l'},
        {"interval", required_argument, 0, 'i'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "?hvc:fW:l:i:", long_options, NULL)) != -1)
    {
        switch (opt)
        {
            case '?':
            case 'h':
                show_usage(EXIT_SUCCESS);
                break;
            case 'v':
                ctx->flags |= F_VERBOSE;
                break;
            case 0: //ttl
                parse_ttl(optarg, ctx);
                break;
            case 'c':
                parse_count(optarg, ctx);
                break;
            case 'f':
                ctx->flags |= F_FLOOD;
                break;
            case 'W':
                parse_linger(optarg, ctx);
                break;
            case 'l':
                parse_preload(optarg, ctx);
                break;
            case 'i':
                parse_interval(optarg, ctx);
                break;
            default:
                show_usage(EXIT_SUCCESS);
        }
    }

    ft_printf("OPTIONS set to TTL=%i LINGER=%i PRELOAD=%i\n", ctx->ttl, ctx->linger, ctx->preload);

    if (optind == argc){
        ft_dprintf(STDERR_FILENO, "ft_ping: usage error: Destination address required\n");
        exit(0);
    }
    ctx->hostname = ft_strdup(argv[argc-1]);
}
