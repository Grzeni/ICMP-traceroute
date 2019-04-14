#pragma once



//a structure for storing important data from the received packages
struct tuple
{
    char ip_addr[20]; //the IP address
    int packet_status; //flag to check what kind of ICMP packet it is (echo request or echo reply)
    int received; //flag to check if this IP_address has been already received
};