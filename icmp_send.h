#pragma once
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
#include "utils.h"


u_int16_t compute_icmp_checksum(const void *buff, int length);

struct icmphdr construct_header(int seq, int id);

void send_packet(int seq, int ttl, int id, const int sockfd, const char *dest_addr);