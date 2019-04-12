#include "../myunp.h"

int main(int argc, char **argv){
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_un cliaddr, servaddr;
    void sig_chld(int);

    listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);

    Unlink(UNIXSTR_PATH);
    bzero(&servaddr, sizeof(struct sockaddr_un));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH);

    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    signal(SIGCHLD, sig_chld);

    while (1){
        clilen = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0){
            if (errno = EINTR)
                continue;
            else
                err_sys("accept error");
        }
        if ((childpid = fork()) == 0){
            close(listenfd);
            ssize_t n;
            char buf[MAXLINE];
            again:
                if ((n = read(connfd, buf, MAXLINE)) < 0 && errno == EINTR){
                    goto again;
                }
                else if (n < 0){
                     err_sys("strecho: read error");
                }
                else{
                    write(connfd, buf, n);
                }
       }
    }
}