#include "../myunp.h"

int main(int argc, char **argv){
    int listenfd, connfd, udpfd, maxfd, nready;
    char msg[MAXLINE];
    pid_t childpid;
    ssize_t n;
    socklen_t len;
    fd_set rset;
    const int on = 1;
    struct sockaddr_in cliaddr, servaddr;
    void sig_chld(int);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_family = AF_INET;

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERV_PORT);

    bind(udpfd, (SA *) &servaddr, sizeof(servaddr));

    maxfd = max(udpfd, listenfd);
    FD_ZERO(&rset);
    signal(SIGCHLD, sig_chld);
    while (1){
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
        if ((nready = select(maxfd + 1, &rset, NULL, NULL, NULL)) < 0){
            if (errno = EINTR)
                continue;
            else
                err_sys("select error");
            
        }

        if (FD_ISSET(listenfd, &rset)){
            len = sizeof(cliaddr);
            connfd = accept(listenfd, (SA *) &cliaddr, &len);
            if ((childpid = fork()) == 0){
                close(listenfd);
                char buf[MAXLINE];
                again:
                    while ((n = read(connfd, buf, MAXLINE)) > 0){
                        write(connfd, buf, n);
                    }
                    if (n < 0 && errno == EINTR){
                        goto again;
                    }
                    else if (n < 0)
                        err_sys("str_cli: server terminated prematurely");
                    exit(0);
            }
            close(connfd);
        }
        if (FD_ISSET(udpfd, &rset)){
            len = sizeof(cliaddr);
            recvfrom(udpfd, msg, MAXLINE, 0, (SA *)&cliaddr, &len);
            sendto(udpfd, msg, MAXLINE, 0, (SA *) &cliaddr, len);
        }
    }
}