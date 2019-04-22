#include "../unpthread.h"

static void *doit(void *);

int main(int argc, char **argv){
    int listenfd, *iptr;
    pthread_t tid;
    socklen_t addrlen, len;
    struct sockaddr *cliaddr;

    if (argc == 2)
        listenfd = tcp_listen(NULL, argv[1], &addrlen);
    else if (argc == 3)
        listenfd = tcp_listen(argv[1], argv[2], &addrlen);
    else 
        err_quit("usage: tcpserv01 [ <host> ] <service or port>");

    cliaddr = malloc(addrlen);
    while (1){
        len = addrlen;
        iptr = malloc(sizeof(int));

        *iptr = accept(listenfd, cliaddr, &len);
        Pthread_create(&tid, NULL, &doit, iptr);
    }
}

static void *doit(void *arg){
    int connfd;
    connfd = *((int *) arg);
    free(arg);
    Pthread_detach(pthread_self());
    ssize_t n;
    char buf[MAXLINE];
    again:
        while ((n = read((int) arg, buf, MAXLINE)) > 0)
            write((int) arg, buf, n);
        if (n < 0 && errno == EINTR)
            goto again;
        else if (n < 0)
            err_sys("echo_str: error");
    close((int) arg);
    return NULL;
}