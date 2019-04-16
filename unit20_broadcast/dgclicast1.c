#include "../myunp.h"

static void recvfrom_alarm(int);

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen);

int main(int argc, char **argv){
    int sockfd;
    struct sockaddr_in servaddr;
    if (argc != 2){
        err_quit("usage: udpcli <IPaddress>");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));
    exit(0);
}

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen){
    int n;
    const int on = 1;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    socklen_t len;
    struct sockaddr *preply_addr;

    preply_addr = Malloc(servlen);
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    signal(SIGALRM, recvfrom_alarm);

    while (Fgets(sendline, MAXLINE, fp) != NULL){
        sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        alarm(5);
        while (1){
            len = servlen;
            n = recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
            if (n < 0){
                if (errno == EINTR)
                    break;
                else
                    err_sys("recvfrom error");
            }else{
                recvline[n] = 0;
                printf("from %s: %s", sock_ntop_host(preply_addr, len), recvline);
            }
        }
    }   
    free(preply_addr);
}

static void recvfrom_alarm(int signo){
    return ;
}