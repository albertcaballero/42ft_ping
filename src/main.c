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

void ping_loop(int sockfd, t_ping *ping){
    int sent = 0, received = 0, seq = 0, flag = 0;
    struct s_timings tmg;
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

    struct timespec send_time, rec_time, start_time;
    double pckt_msec =0;
    unsigned long total_msec = 0;
    clock_gettime(CLOCK_REALTIME, &start_time);

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

        flag = 1;
        clock_gettime(CLOCK_REALTIME, &send_time);
        if (sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&ping_to, (socklen_t)sizeof(ping_to)) < 0){
            perror("ERROR SENDING");
            flag = 0;
        }
        sent++;

        int rec_bytes = recvfrom(sockfd, r_buffer, sizeof(r_buffer), 0, (struct sockaddr *)&ping_from, &addrlen);
        if (rec_bytes < 0){
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                printf("Request timeout for icmp_seq %d\n", seq);
            else
                perror("ERROR RECEIVING");
        }else{
            if (!flag)
                continue;

            char cha[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ping_from.sin_addr), cha, INET_ADDRSTRLEN);

            struct iphdr *ip_hdr = (struct iphdr *)r_buffer;
            // struct icmphdr *icmp_reply = (struct icmphdr *)(r_buffer + ip_hdr->ihl * 4);

            clock_gettime(CLOCK_REALTIME, &rec_time);
            double timeElapsed = ((double)(rec_time.tv_nsec - send_time.tv_nsec)) / 1000000.0;
            pckt_msec = (rec_time.tv_sec - send_time.tv_sec) * 1000.0 + timeElapsed;

            printf("%d bytes from %s (%s): icmp_seq=%i ttl=%i time=%.2f ms\n",
                ntohs(ip_hdr->tot_len) - (ip_hdr->ihl * 4),
                ping->hostname, cha, seq, ip_hdr->ttl, pckt_msec);

            received++;
            total_msec+=pckt_msec;
            update_timings(&tmg, pckt_msec);
        }
        sleep(1);
    }

    clock_gettime(CLOCK_REALTIME, &rec_time);
    double timeElapsed = ((double)(rec_time.tv_nsec - start_time.tv_nsec)) / 1000000.0;
    total_msec = (rec_time.tv_sec - start_time.tv_sec) * 1000.0 + timeElapsed;

    unsigned int loss = 0;
    if (seq)
        loss = (seq - received)/seq * 100;
    calc_endtimes(&tmg);
    ft_printf("\n--- %s ping statistics ---\n", ping->hostname);
    ft_printf("%u packets transmitted, %u received, %u%% packet loss, time %ums\n", sent, received, loss, total_msec);
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", tmg.min, tmg.avg, tmg.max, tmg.mdev);
}

int main(int argc, char** argv){
    t_ping ping;
    int flags = 0;
    int sock_r;

    int idx = 0;
    parse_args(argc, argv, &ping);
 //    for (int i = 1; i < argc; ++i){
	// 	set_flags(argv[i], &flags);
	// 	idx = argv[i][0] != '-';
	// }
    ping.flags = flags;
    if (ping.flags & F_HELP){
        show_usage(0);
    }
return 0;
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

    if (ping.flags & F_VERBOSE){
        //TODO
        ft_printf("FT_PING %s (%s) %i(84) bytes of data.\n", ping.hostname, ping.ip, PACKET_SIZE);
    }

    ft_printf("FT_PING %s (%s) %i(84) bytes of data.\n", ping.hostname, ping.ip, PACKET_SIZE);
    ping_loop(sock_r, &ping);

    close(sock_r);
    free(ping.ip);
    free(ping.hostname);
}

//TODO list
// implement -verbose
// implement other flags
// fix ping result and summary
// timings mdev
// fit to inetutils ping (not yours)
