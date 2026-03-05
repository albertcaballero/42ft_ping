#include "../ft_ping.h"
#include <netdb.h>


char* gethost(char *argv){
    struct addrinfo hints;
    struct addrinfo *results;
    char *ip;

    ft_bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    if (getaddrinfo(argv, NULL, &hints, &results) < 0){
        //TODO error
        return NULL;
    }

    ip = ft_strdup(results->ai_addr->sa_data);

    freeaddrinfo(results);
    return ip;
}
