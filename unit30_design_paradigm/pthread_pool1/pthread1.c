#include "../../unpthread.h"
#include "pthread1.h"

void thread_make(int i){
    void *thread_main(void *);
    Pthread_create(&tprt[i].thread_tid, NULL, &thread_main, (void *) i);
    return ;
}

void *thread_main(void *arg){
    int connfd;
    void web_child(int);
    socklen_t clilen;
    struct sockaddr *cliaddr;

    cliaddr = malloc(addrlen);

    printf("thread %d starting\n", (int) arg);
    while (1){
        clilen = addrlen;
        Pthread_mutex_lock(&mlock);
        connfd = accept(listenfd, cliaddr, &clilen);
        Pthread_mutex_unlock(&mlock);
        tprt[(int) arg].thread_count++;

        web_child(connfd);
        close(connfd);
    }
}