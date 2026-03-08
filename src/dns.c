#include "../ft_ping.h"
#include <netdb.h>


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
