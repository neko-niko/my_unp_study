#include "../myunp.h"

static int nchildren;
static pid_t *pids;

int main(int argc, char **argv){

    long *cptr, meter(int);
    int listenfd, i;
    socklen_t addrlen;
    void sig_int(int);
    pid_t child_make(int, int, int, long *);
    
    if (argc == 3)
        listenfd = tcp_listen(NULL, argv[1], &addrlen);
    else if (argc == 4)
        listenfd = tcp_listen(argv[1], argv[2], &addrlen);
    else 
        err_quit("usage: preparatory_fork_serv [ <host> ] <port#> <#children>");

    nchildren = atoi(argv[argc - 1]);
    pids = Calloc(nchildren, sizoef(pid_t));
    cptr = meter(nchildren);

    for (i = 0; i < nchildren; i++){
        pids[i] = child_make(i, listenfd, addrlen, cptr);
    }

    signal(SIGINT, sig_int);

    while (1){
        pause();
    }    
}

void sig_int(int signo){
    int i;
    void pr_cpu_time(void);

    for (i = 0;i < nchildren; i++)
        kill(pids[i], SIGTERM);

    while (wait(NULL) > 0)
        ;

    if (errno != ECHILD)
        err_sys("wait error");

    pr_cpu_time();
    exit(0);
}


pid_t child_make(int i, int listenfd, int addrlen, long *cptr){
    pid_t pid;
    void child_main(int, int, int, long *);

    if ((pid = fork()) > 0)
        return (pid);

    child_main(i, listenfd, addrlen, cptr);

}

void child_main(int i, int listenfd, int addrlen, long *cptr){
    int connfd;
    void web_child(int);
    socklen_t clilen;
    struct sockaddr *cliaddr;

    cliaddr = malloc(addrlen);

    printf("child %ld starting\n", (long) getpid());
    while (1){
        clilen = addrlen;
        connfd = accept(listenfd, cliaddr, &clilen);
        cptr[i] += 1;
        web_child(connfd);
        close(connfd);
    }
}