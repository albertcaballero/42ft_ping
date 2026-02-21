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
    if (sock_r < 0){
        perror("ERR creating socket");
        return -1; //TODO error
    }

    if (setsockopt(sock_r, IPPROTO_IP, IP_TTL, &tt_val, sizeof(tt_val)) != 0){
        perror("ERR creating socket");
        return -1; //TODO error
    }

    timeout.tv_sec = RECV_TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(sock_r, SOL_SOCKET, SO_RCVTIMEO, (const void*)&timeout, sizeof(timeout)) != 0){
        perror("ERR creating socket");
        return -1; //TODO error
    }
    return sock_r;
}

void show_summary(t_ping *ping){
    ft_printf("\n--- %s ping statistics ---\n", ping->ip);
    ft_printf("%u packets transmitted, %u received, %u%% packet loss, time %u\n", 123, 123, 123, 123);
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", 123.0, 123.0, 123.0, 123.0);
}

void packet_string(){
    //TODO
    printf("%i bytes from %s (%s): icmp_seq=%i ttl=%i time=%.2f ms\n",
            64, "188.114.96.5", "188.114.96.5", 1, 1, 1.0);
}

void ping_loop(int sockfd, t_ping *ping){
    (void) ping;
    int sent = 0, received = 0, seq = 0;

    //sendto
    t_packet packet;
    struct sockaddr_in ping_to;
    ping_to.sin_family = AF_INET;
    ping_to.sin_port = htons(0);
    inet_aton(ping->ip, &ping_to.sin_addr);
    memset(ping_to.sin_zero, 0, sizeof(ping_to.sin_zero));
    // ping_to.sin_addr.s_addr = inet_addr(ping->ip);

    //recvfrom
    char r_buffer[128];
    struct sockaddr_in ping_from;
    socklen_t addrlen;

    struct timespec send_time, rec_time;

    while (pinging){
        seq++;

        ft_bzero(&packet, sizeof(packet));
        packet.header.type = ICMP_ECHO; //8 echo request
        packet.header.code = 0;
        packet.header.checksum = 0; //TODO checksum
        packet.header.un.echo.id = 0; //TODO id
        packet.header.un.echo.sequence = seq; //TODO sequence
        ft_bzero(&packet.msg, PACKET_SIZE);

        clock_gettime(CLOCK_REALTIME, &send_time);
        if (sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&ping_to, (socklen_t)sizeof(ping_to)) < 0){
            perror("ERROR SENDING");
        }
        sent++;

        if (recvfrom(sockfd, r_buffer, sizeof(r_buffer), 0, (struct sockaddr *)&ping_from, &addrlen) < 0){
            perror("ERROR RECEIVING\n");
        }else{
            //check if ping_from is the one we want
            received++;
        }
        clock_gettime(CLOCK_REALTIME, &rec_time);
        send_time.tv_sec -= rec_time.tv_sec;
        send_time.tv_nsec -= rec_time.tv_nsec;
        packet_string();
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
    //dns or reverse dns
    ping.ip = ip;

    ft_printf("PING %s (%s) %i(84) bytes of data.\n", ip, ip, PACKET_SIZE);
    sock_r = create_socket();
    if (sock_r < 0){
        ft_dprintf(2, "Error creating socket\n");
        return 0;
    }
    signal(SIGINT, sigint_handler);

    ping_loop(sock_r, &ping);
    show_summary(&ping);
    //cleanup();
}
