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
#include "icmp_send.h"
#include "utils.h"


int main(int argc, const char *argv[])
{
    //input validation
    if (argc != 2)
    {
        fprintf(stderr, "invalid arguments: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    //creating a raw socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //received packages counter
    int received_packets = 0;

    //the packages id will be the pid
    uint8_t id = (uint8_t)getpid();

    //flag to check whether we reached the destination
    int dest_reached = 0;

    //incrementing the ttl with every loop iteration sending 3 ICMP echo request packets
    for (int ttl = 1; ttl < 31 && dest_reached != 1; ttl++)
    {

        for (uint16_t i = 0; i < 3; i++)
        {

            send_packet(ttl, ttl, id, sockfd, argv[1]);
        }

        //using select to avoid active waiting
        fd_set descriptors;
        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        //resetting the received packages counter
        received_packets = 0;

        //counter to get the times of each packet
        int ptimes = 0;

        //an array for storing info about the IP addresses of the received packages
        struct tuple ip_array[3] = {0};

        while (received_packets < 3)
        {
            int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);

            if (ready < 0)
            {
                fprintf(stderr, "select_error");
            }
            if (ready == 0)
            {
                break;
            }
            else
            {
                ip_array[received_packets] = receive_packet(sockfd, ttl, id);
                ptimes += (1000 - tv.tv_usec / 1000);

                //destination reached
                if (ip_array[received_packets].packet_status == 1)
                {

                    dest_reached = 1;
                    ip_array[received_packets].received = 1;
                    received_packets += 1;

                } //destination still not reached
                else if (ip_array[received_packets].packet_status == 0)
                {
                    ip_array[received_packets].received = 1;
                    received_packets += 1;
                }
            }
        }

        //printing the output

        if (received_packets == 0)
        {
            printf("*\n");
            continue;
        }
        for (int i = 0; i < 3; i++)
        {
            if (ip_array[i].received == 1)
                printf("%s ", ip_array[i].ip_addr);

            for (int j = i; j < 3; j++)
            {
                if (strcmp(ip_array[i].ip_addr, ip_array[j].ip_addr) == 0)
                    ip_array[j].received = 0;
            }
        }
        if (received_packets < 3)
        {
            printf("???\n");
        }
        else
        {
            ptimes = ptimes / 3;
            printf("%dms\n", ptimes);
        }
    }
}