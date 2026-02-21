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
    //TODO
    ft_printf("\n--- %s ping statistics ---\n", ping->ip);
    ft_printf("%u packets transmitted, %u received, %u%% packet loss, time %u\n", 123, 123, 123, 123);
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", 123.0, 123.0, 123.0, 123.0);
}

uint16_t calculate_checksum(uint16_t seq) {
    uint32_t sum = 0;

    sum += (8 << 8) | 0;   // Type + Code
    sum += 0x0000;         // Checksum field
    sum += 1313;           // Identifier
    sum += seq;            // Sequence

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (uint16_t)(~sum);
}

void ping_loop(int sockfd, t_ping *ping){
    int sent = 0, received = 0, seq = 0;

    //sendto
    t_packet packet;

    struct sockaddr_in ping_to;
    ping_to.sin_family = AF_INET;
    ping_to.sin_port = 0;
    inet_aton(ping->ip, &ping_to.sin_addr);
    memset(ping_to.sin_zero, 0, sizeof(ping_to.sin_zero));

    //recvfrom
    char r_buffer[128];
    struct sockaddr_in ping_from;
    socklen_t addrlen;

    struct timespec send_time, rec_time;
    double pckt_msec =0, total_msec = 0;

    while (pinging){
        seq++;
        ft_bzero(&packet, sizeof(packet));
        packet.header.type = ICMP_ECHO; //8 echo request
        packet.header.code = 0;
        packet.header.checksum = calculate_checksum(seq);
        packet.header.un.echo.id = 1313;
        packet.header.un.echo.sequence = seq;
        ft_bzero(&packet.msg, PACKET_SIZE);


        clock_gettime(CLOCK_REALTIME, &send_time);
        if (sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&ping_to, (socklen_t)sizeof(ping_to)) < 0){
            ft_dprintf(2, "holi\n");
            perror("ERROR SENDING");
        }
        sent++;

        addrlen = sizeof(ping_from);
        int rec_bytes = recvfrom(sockfd, r_buffer, sizeof(r_buffer), 0, (struct sockaddr *)&ping_from, &addrlen);
        if (rec_bytes < 0){
            perror("ERROR RECEIVING\n");
        }else{
            char *cha = inet_ntoa(ping_from.sin_addr);
            cha[20] = 0;

            clock_gettime(CLOCK_REALTIME, &rec_time);
            double timeElapsed = ((double)(rec_time.tv_nsec - send_time.tv_nsec)) / 1000000.0;
            pckt_msec = (rec_time.tv_sec - send_time.tv_sec) * 1000.0 + timeElapsed;\

            printf("%i bytes from %s (%s): icmp_seq=%i ttl=%i time=%.2f ms\n",
                    64, ping->ip, "REVDNS", seq, 64, pckt_msec);

            received++;
            total_msec+=pckt_msec;
        }

        sleep(1);
    }
    show_summary(ping);
}

int main(int argc, char** argv){
    int flags = 0;
    char *ip = NULL;
    t_ping ping; //TODO t_ping al carrer substitute with t_program
    int sock_r;

    for (int i = 0; i < argc; ++i){
		set_flags(argv[i], &flags);
	}
    ip = clean_args(argv, argc);
    //TODO dns & reverse dns
    ping.ip = ip;

    ft_printf("PING %s (%s) %i(84) bytes of data.\n", ip, ip, PACKET_SIZE);
    sock_r = create_socket();
    if (sock_r < 0){
        ft_dprintf(2, "Error creating socket\n");
        return 0;
    }
    signal(SIGINT, sigint_handler);

    ping_loop(sock_r, &ping);
    // cleanup(); //TODO
}
