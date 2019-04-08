#include "../myunp.h"

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen);

int main(int argc, char **argv){
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    bind(sockfd, (SA *) &servaddr, sizeof(servaddr));
    dg_echo(sockfd, (SA *) &cliaddr, sizeof(cliaddr));
    exit(0);
}

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen){
    int n;
    socklen_t len;
    char mesg[MAXLINE];

    while (1){
        len = clilen;
        n = recvfrom(sockfd, mesg, MAXLINE, 0, &pcliaddr, &len);
        sendto(sockfd, mesg, n, 0, &pcliaddr, sizeof(pcliaddr));
    }
}   