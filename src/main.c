#include "../ft_ping.h"
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <unistd.h>

/* DOCUMENTATION
 https://www.geeksforgeeks.org/computer-networks/ping-in-c/

 */

int pinging = 1;
void sigint_handler(int sig){
    (void)sig;
    pinging = 0;
}

int create_socket(){
    int sock_r;
    int tt_val = 64;
    struct timeval timeout;

    sock_r = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock_r < 0)
        return -1; //TODO error

    if (setsockopt(sock_r, IPPROTO_IP, IP_TTL, &tt_val, sizeof(tt_val)) != 0){
        return -1; //TODO error
    }

    timeout.tv_sec = RECV_TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(sock_r, SOL_SOCKET, SO_RCVTIMEO, (const void*)&timeout, sizeof(timeout)) != 0){
        return -1; //TODO error
    }
    return sock_r;
}

void show_summary(t_ping *ping){
    ft_printf("\n--- %s ping statistics ---\n", ping->ip);
}

void ping_loop(int sockfd, t_ping *ping){
    int sent = 0, received = 0;
    t_packet packet;
    struct sockaddr ping_to;
    struct sockaddr_in ping_from;
    char r_buffer[128];
    socklen_t addrlen;

    while (pinging){
        ft_bzero(&packet, sizeof(packet));
        packet.header.type = ICMP_ECHO;
        // packet.header.
        ft_bzero(&packet.msg, PACKET_SIZE);

        //CLOCK_GETTIME()
        if (sendto(sockfd, &packet, sizeof(packet), 0, const struct sockaddr *addr, socklen_t addr_len) < 0){
            //error
        }
        sent++;

        if (recvfrom(sockfd, r_buffer, sizeof(r_buffer), 0, (struct sockaddr *)&ping_from, &addrlen) < 0){
            //error
        }else{
            //check if ping_from is the one we want
            received++;
        }
        //CLOCK_GETTIME()

    }
}

int main(int argc, char** argv){
    int flags = 0;
    char *ip = NULL;
    t_ping ping;
    int sock_r;

    for (int i = 0; i < argc; ++i){
		set_flags(argv[i], &flags);
	}
    ip = clean_args(argv, argc);
    ping.ip = ip;


    sock_r = create_socket();
    signal(SIGINT, sigint_handler);

    ping_loop(sock_r, &ping);
    show_summary(&ping);
    //cleanup();
}
