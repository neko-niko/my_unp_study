// #include "../myunp.h"
#include "sys/socket.h"
#include "stdio.h"

int main(){
    int tcp_sock, udp_sock, tcprecv, tcpsend, udpsend;
    socklen_t templen;
    templen = sizeof(int);
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    getsockopt(tcp_sock, SOL_SOCKET, SO_RCVBUF, &tcprecv, &templen);
    templen = sizeof(int);
    getsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, &tcpsend, &templen);
    templen = sizeof(int);
    getsockopt(udp_sock, SOL_SOCKET, SO_SNDBUF, &udpsend, &templen);
    printf("%d, %d, %d", tcprecv, tcpsend, udpsend);
}