#include "../ft_ping.h"
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <unistd.h>

/* DOCUMENTATION
 https://www.geeksforgeeks.org/computer-networks/ping-in-c/
https://beej.us/guide/bgnet/html/split/index.html
 */

int pinging = 1;
void sigint_handler(int sig){
    (void)sig;
    pinging = 0;
}

int create_socket(){
    int sock_r;
    int tt_val = 64;
    struct timeval timeout = {RECV_TIMEOUT, 0};

    sock_r = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock_r < 0){
        perror("ERR creating socket");
        return -1;
    }

    if (setsockopt(sock_r, IPPROTO_IP, IP_TTL, &tt_val, sizeof(tt_val)) != 0){
        perror("ERR creating socket");
        return -1;
    }

    if (setsockopt(sock_r, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != 0){
        perror("ERR creating socket");
        return -1;
    }
    return sock_r;
}

uint16_t calculate_checksum(void *data, int len) {
    unsigned short *buf = data;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void ping_loop(int sockfd, t_program *ping){
    int sent = 0, received = 0, seq = 0;

    //sendto
    t_packet packet;

    struct sockaddr_in ping_to;
    ping_to.sin_family = AF_INET;
    ping_to.sin_port = htons(0);
    if (!inet_pton(AF_INET, ping->ip, &(ping_to.sin_addr))){
        ft_dprintf(2, "Wrong address\n");
        return;
    }
    ft_memset(ping_to.sin_zero, 0, sizeof(ping_to.sin_zero));

    //recvfrom
    char r_buffer[128];
    struct sockaddr_in ping_from;
    socklen_t addrlen = sizeof(ping_from);

    struct timespec send_time, rec_time;
    double pckt_msec =0;
    unsigned long total_msec = 0;

    while (pinging){
        seq++;
        //TODO put this in function
        ft_bzero(&packet, sizeof(packet));
        packet.header.type = ICMP_ECHO; //8 echo request
        packet.header.code = 0;
        packet.header.un.echo.id = getpid();
        packet.header.un.echo.sequence = seq;
        ft_bzero(&packet.msg, PACKET_SIZE);
        packet.header.checksum = 0;
        packet.header.checksum = calculate_checksum(&packet, sizeof(packet));

        clock_gettime(CLOCK_REALTIME, &send_time);
        if (sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&ping_to, (socklen_t)sizeof(ping_to)) < 0){
            perror("ERROR SENDING");
        }
        sent++;

        int rec_bytes = recvfrom(sockfd, r_buffer, sizeof(r_buffer), 0, (struct sockaddr *)&ping_from, &addrlen);
        if (rec_bytes < 0){
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Request timeout for icmp_seq %d\n", seq);
            }else
                perror("ERROR RECEIVING");
        }else{
            char cha[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ping_from.sin_addr), cha, INET_ADDRSTRLEN);

            clock_gettime(CLOCK_REALTIME, &rec_time);
            double timeElapsed = ((double)(rec_time.tv_nsec - send_time.tv_nsec)) / 1000000.0;
            pckt_msec = (rec_time.tv_sec - send_time.tv_sec) * 1000.0 + timeElapsed;

            printf("%i bytes from %s (%s): icmp_seq=%i ttl=%i time=%.2f ms\n",
                    64, ping->hostname, cha, seq, 64, pckt_msec);

            received++;
            total_msec+=pckt_msec;
        }
        sleep(1);
    }
    unsigned int loss = 0;
    if (seq)
        loss = (seq - received)/seq * 100;
    ft_printf("\n--- %s ping statistics ---\n", ping->hostname);
    ft_printf("%u packets transmitted, %u received, %u%% packet loss, time %ums\n", sent, received, loss, total_msec);
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", 123.0, 123.0, 123.0, 123.0);
}

int main(int argc, char** argv){
    t_program ping;
    int flags = 0;
    int sock_r;

    int idx = 0;
    for (int i = 1; i < argc; ++i){
		set_flags(argv[i], &flags);
		idx = argv[i][0] != '-';
	}
    ping.ip = gethost(argv[idx]);
    if (!ping.ip){
        return 0;
    }
    ping.hostname = ft_strdup(argv[idx]);

    sock_r = create_socket();
    if (sock_r < 0){
        return 0;
    }
    signal(SIGINT, sigint_handler);

    //TODO implement help and verbose
    ft_printf("FT_PING %s (%s) %i(84) bytes of data.\n", ping.hostname, ping.ip, PACKET_SIZE);
    ping_loop(sock_r, &ping);

    // cleanup(); //TODO
    close(sock_r);
    free(ping.ip);
    free(ping.hostname);
}

//TODO list
// implement --help and -verbose
// cleanup (free and other)
// fix struct s_ping
// fix ping result and summary
//
