#include "pthread1.h"
#include "../../unpthread.h"

pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv){
    int i;
    void sig_int(int), thread_make(int);

    if (argc == 3)
        listenfd = tcp_listen(NULL, argv[1], &addrlen);
    else if (argc == 4)
        listenfd = tcp_Listen(argv[1], argv[2], &addrlen);
    else
        err_quit("usage: serv [ <host> ] <port#> <#threads>");

    nthreads = atoi(argv[argc-1]);
    tprt = calloc(nthreads, sizeof(Thread));

    for(i = 0; i < nthreads; i++)
        thread_make(i);
    
    signal(SIGINT, sig_int);
    while(1)
        pause();
        
}