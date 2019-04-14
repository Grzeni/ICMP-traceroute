/* Grzegorz Winiarski 288337 */

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <sys/select.h>

//computes the ICMP header checksum
u_int16_t compute_icmp_checksum(const void *buff, int length)
{
    u_int32_t sum;
    const u_int16_t *ptr = buff;
    assert(length % 2 == 0);
    for (sum = 0; length > 0; length -= 2)
        sum += *ptr++;
    sum = (sum >> 16) + (sum & 0xffff);
    return (u_int16_t)(~(sum + (sum >> 16)));
}

//creates the ICMP header
struct icmphdr construct_header(int seq, int id)
{
    struct icmphdr icmp_header;
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.un.echo.id = id;
    icmp_header.un.echo.sequence = seq;
    icmp_header.checksum = 0;
    icmp_header.checksum = compute_icmp_checksum(
        (u_int16_t *)&icmp_header,
        sizeof(icmp_header));

    return icmp_header;
}

//sends the packet to the destination with a specified sequence, ttl, id and socket
void send_packet(int seq, int ttl, int id, const int sockfd, const char *dest_addr)
{
    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;

    if (inet_pton(AF_INET, dest_addr, &recipient.sin_addr) <= 0) {
        fprintf(stderr, "invalid IP address or sth else: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } 

    struct icmphdr new_hdr = construct_header(seq, id);

    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
    
    sendto(
        sockfd,
        &new_hdr,
        sizeof(new_hdr),
        0,
        (struct sockaddr *)&recipient,
        sizeof(recipient));
}