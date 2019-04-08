#include "../myunp.h"

void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv){
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
    str_cli(stdin, sockfd);
    exit(0);
}

void str_cli(FILE *fp, int sockfd){
    int maxfdp1;
    fd_set rset;
    char recvline[MAXLINE], sendline[MAXLINE];
    FD_ZERO(&rset);
    while (1){
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        printf("%s", "iter!\n");

        maxfdp1 = max(fileno(fp), sockfd) + 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)){
            printf("%s", "sock_read\n");
            if (readline(sockfd, recvline, MAXLINE) == 0){
                err_quit("str_cli: server  terminated prematurely");
            }
            printf("%s", recvline);
            fputs(recvline, stdout);
        }
        if (FD_ISSET(fileno(fp), &rset)){
            printf("%s", "stdin insert\n");
            if (fgets(sendline, MAXLINE, fp) == NULL){
                return ;
            }
            writen(sockfd, sendline, sizeof(sendline));
        }
    }
}