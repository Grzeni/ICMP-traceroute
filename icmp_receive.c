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
#include "icmp_receive.h"


//receives the ICMP packet using the recvfrom function
struct tuple receive_packet(const int sockfd, int ttl, int pid)
{
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    u_int8_t buffer[IP_MAXPACKET];

    ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr *)&sender, &sender_len);
    if (packet_len < 0)
    {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct tuple res;

    inet_ntop(AF_INET, &(sender.sin_addr), res.ip_addr, sizeof(res.ip_addr));

    struct iphdr *ip_header = (struct iphdr *)buffer;
    u_int8_t *icmp_packet = buffer + 4 * ip_header->ihl;
    struct icmphdr *icmp_header = (struct icmphdr *)icmp_packet;

    if (icmp_header->type == ICMP_ECHOREPLY)
    {
        //destination reached, so let's check if the packet is a reply to our packet
        int sequence_num = icmp_header->un.echo.sequence;
        int package_id = icmp_header->un.echo.id;

        if (sequence_num == ttl && package_id == pid)
        {
            res.packet_status = 1;
        }
    }
    //destination not reached, so we have to go deeper to get the sequence and id to 
    //make sure this is a reply to our packet
    else if (icmp_header->type == ICMP_TIME_EXCEEDED)
    {
        
        struct iphdr *inner_ip_header = (struct iphdr *)((uint8_t *)icmp_header + sizeof(icmp_header));

        struct icmphdr *inner_icmp_header = (struct icmphdr *)((uint8_t *)icmp_header + sizeof(icmp_header) + 4 * inner_ip_header->ihl);

        int sequence_num = inner_icmp_header->un.echo.sequence;

        int package_id = inner_icmp_header->un.echo.id;

        if (package_id == pid && sequence_num == ttl)
        {
            res.packet_status = 0;
        }
    }
    else
    {
        res.packet_status = -1;
    }
    return res;
}
