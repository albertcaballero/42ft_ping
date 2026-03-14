#include "../ft_ping.h"
#include <bits/getopt_core.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void set_flags(char* arg, int *flags){
	if (!arg || ft_strlen(arg) == 0){
		return;
	}
	if (arg[0] != '-'){
		return;
	}
	for (int i = 1; arg[i] != 0; ++i){
		switch (arg[i]){
			case 'v':
				*flags |= F_VERBOSE;
				break;
			case 'h':
			case '?':
				*flags |= F_HELP;
				break;
			default:
				ft_dprintf(2, "ft_ping: invalid option -- '%c'\n", arg[i]);
				exit(2);
				break;
		}
	}
}

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

void parse_args(int argc, char *argv[], t_ping* ctx)
{
    int         opt;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'},
        {"ttl", required_argument, 0, 't'},
        {"count", required_argument, 0, 'c'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "?hvt:c:", long_options, NULL)) != -1)
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
            case 't':
                parse_ttl(optarg, ctx);
                break;
            case 'c':
                parse_count(optarg, ctx);
                break;
            default:
                show_usage(EXIT_SUCCESS);
        }
    }

    for (int i = optind; i < argc; i++) {
        printf("ARG=%s\n", argv[i]);
    }
    // if (ctx->dst == NULL) {
    //     fprintf(stderr, "ft_ping: Missing host operand.\n");
    //     show_usage(EXIT_FAILURE);
    // }

}
