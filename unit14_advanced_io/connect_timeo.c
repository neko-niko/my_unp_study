#include "../myunp.h"

typedef void Sigfunc(int);

static void connect_alarm(int);


int connect_timeo(int sockfd, const SA *saptr, socklen_t salen, int nsec){
    Sigfunc *sigfunc;
    int n;
    sigfunc = signal(SIGALRM, connect_alarm);
    if (alarm(nsec) != 0){
        err_msg("connect_timeo: alarm was already set");
    }

    if ((n = connect(sockfd, saptr, salen)) < 0){
        close(sockfd);
        if (errno == EINTR)
            errno = ETIMEDOUT;
    }
    alarm(0);       //关闭定时器
    signal(SIGALRM, sigfunc);   //回复sigalrm的信号处理函数    

    return n;
}

static void connect_alarm(int signo){
    return ;
}