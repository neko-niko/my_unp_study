#include "../myunp.h"

void str_echo(int sockfd);
static void sig_urg(int), sig_alrm(int);
void heartbeat_serv(int , int, int);

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
    heartbeat_serv(sockfd, 1, 5);
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



//心跳函数实现
static int servfd;
static int nsec;
static int maxnalarms;
static int nprobes;

void heartbeat_serv(int servfd_arg, int nsec_arg, int maxnalarms_arg){
    servfd = servfd_arg;
    if ((nsec = nsec_arg) < 1)
        nsec = 1;
    
    if ((maxnalarms = maxnalarms_arg) < nsec)
        maxnalarms = nsec;

    signal(SIGURG, sig_urg);
    Fcntl(servfd, __F_SETOWN, getpid());

    signal(SIGALRM, sig_alrm);
    alarm(nsec);
}



static void sig_urg(int singo){
    int n;
    char c;
    if ((n = recv(servfd, &c, 1, MSG_OOB)) < 0){
        if (errno != EWOULDBLOCK)
            err_sys("recv error");
    }

    send(servfd, &c, 1, MSG_OOB);
    nprobes = 0;
    return ;
}

static void sig_alrm(int singo){
    if (++nprobes > maxnalarms){
        printf("no probes from client\n");
        exit(0);

    }
    alarm(nsec);
    return ;
    
}