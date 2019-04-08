// #include "unp.h"
#include "../myunp.h"
void str_echo(int sockfd);

int main(int argc, char **argv){
    int listenfd, connfd;
    struct sockaddr_in cliaddr, servaddr;
    socklen_t clilen;
    pid_t cpid;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);
    while (1){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (SA *)&cliaddr, &clilen);
        if (connfd < 0){
            if (errno == EINTR)
                continue;
            else
                err_sys("accept error");
        }
        if ((cpid = fork()) == 0){
            Close(listenfd);
            str_echo(connfd);
            exit(0);

        }
        Close(connfd);
    }
}

void str_echo(int sockfd){
    ssize_t n;
    char buf[MAXLINE];
    again:
        while ((n = read(sockfd, buf, MAXLINE)) > 0){
            printf("%s", buf);
            Write(sockfd, buf, n);
        }
        if (n < 0 && errno == EINTR)
            goto again;
        else if (n < 0)
            err_sys("strecho: read error");
}