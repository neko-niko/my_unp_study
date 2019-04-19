#include "../myunp.h"

void heartbeat_cli(int, int, int);
static void sig_urg(int singo);
static void sig_alrm(int singo);
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
    int n, i;
    stdineof = 0;
    FD_ZERO(&rset);
    heartbeat_cli(sockfd, 1, 5);
    while(1){
        if (stdineof == 0){
            FD_SET(fileno(fp), &rset);
        }
        FD_SET(sockfd, &rset);
        maxfdp1 = max(sockfd, fileno(fp)) + 1;
        i = select(maxfdp1, &rset, NULL, NULL, NULL);
        if (i < 0){
            if (errno == EINTR)
                continue;
            else{
                printf("error in select");
                break;
            }
        }
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



//以下是对心跳功能的客户端实现

static int  servfd;
static int  nsec;       //sigalrm的频率
static int  maxnprobes; //在客户认为服务器或连接不复存活之前处理的无服务器响应的siglarm总数
static int nprobes;     //计量从收到来自服务器的最后一个应答来处理sigalrm数目

void heartbeat_cli(int servfd_arg, int nsec_arg, int maxnprobes_arg){
    servfd = servfd_arg;
    if ((nsec = nsec_arg) < 1)
        nsec = 1;
    if ((maxnprobes = maxnprobes_arg) < nsec)
        maxnprobes = nsec;
    
    nprobes = 0;
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

    nprobes = 0;
    return ;
}

static void sig_alrm(int singo){
    if (++nprobes > maxnprobes){
        fprintf(stderr, "server is unreachable\n");
        exit(0);
    }
    send(servfd, "1", 1, MSG_OOB);
    alarm(nsec);
    return ;
}