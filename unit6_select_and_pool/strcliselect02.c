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
    int maxfdp1, stdineof;
    fd_set rset;
    char buf[MAXLINE];
    int n;
    stdineof = 0;
    FD_ZERO(&rset);

    while(1){
        if (stdineof == 0){
            FD_SET(fileno(fp), &rset);
        }
        FD_SET(sockfd, &rset);
        maxfdp1 = max(sockfd, fileno(fp)) + 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(sockfd, &rset)){
            if ((n = Read(sockfd, buf, MAXLINE)) == 0){
                if (stdineof == 1)
                    return; 
                else
                    err_quit("str_cli: server terminated prematurely");
            }
            Write(fileno(stdout), buf, n);
        }
        if (FD_ISSET(fileno(fp), &rset)){
            if ((n = Read(fileno(fp), buf, MAXLINE)) == 0){
                printf("%s", "close writen");
                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            printf("%s", buf);
            Writen(sockfd, buf, n);
        }
    }
}