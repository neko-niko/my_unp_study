#include "./icmpd.h"

int main(int argc, char **argv){
    int i, sockfd;
    struct sockaddr_un sun;

    if (argv != 1)
        err_quit("usage: icmpd");

    maxi = -1;
    for (i = 0; i<FD_SETSIZE; i++)
        client[i].connfd = -1;
    FD_ZERO(&allset);

    fd4 = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    FD_SET(fd4, &allset);
    maxfd = fd4;

#ifdef IPV6
    fd6 = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)
    FD_SET(fd6, &allset);
    maxfd = max(fd4, fd6);
#endif

    listenfd = socket(AF_UNIX, SOCK_STREAM, 0);
    sun.sun_family = AF_LOCAL;
    strcpy(sun.sun_path, ICMPD_PATH);
    unlink(ICMPD_PATH);
    bind(listenfd, (SA *) &sun, sizeof(sun));
    listen(listenfd, LISTENQ);
    FD_SET(listenfd, &allset);
    maxfd = max(maxfd, listenfd);

    while (1){
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &rset))
            if (readable_listen() <= 0)
                continue;
            
            if (FD_ISSET(fd4, &rset))
                if (readable_v4() <= 0)
                    continue;
#ifdef IPV6
            if (FD_ISSET(fd6, &rset))
                if (readable_v6() <= 0)
                    continue;
#endif
            for (i = 0; i <= maxi; i++){        //测试每个已连接unix域套件字， 其中任何一个变为可读意味着响应客户已发送一个描述符
                if ((sockfd = client[i].connfd) < 0)
                    continue;
                if (FD_ISSET(sockfd, &rset))
                    if (readable_conn(i) <= 0)
                        break;
            }
    }
    exit(0);
}

int readable_listen(void){
    int i, connfd;
    socklen_t clilen;

    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (SA *) &cliaddr, &clilen);

    for (i = 0; i < FD_SETSIZE; i++){
        if (client[i].connfd < 0){
            client[i].connfd = connfd;
            break;
        }
    }

    if (i == FD_SETSIZE){
        close(connfd);
        return (--nready);
    }

    printf("new connection, i = %d, connfd = %d\n", i, connfd);

    FD_SET(connfd, &rset);
    if (connfd > maxfd)
        maxfd = connfd;
    if (i > maxi)
        maxi = i;
    
    return (--nready);
}


int readable_conn(int i){
    int unixfd, recvfd;
    char c;
    ssize_t n;
    socklen_t len;
    struct sockaddr_storage ss;
    unixfd = client[i].connfd;
    recvfd = -1;
    if ((n = read_fd(unixfd, &c, 1, &recvfd)) == 0){
        err_msg("client %d terminated, recvfd = %d", i, recvfd);
        goto clientdone;
    }

    if (recvfd < 0){
        err_msg("read_fd did not return descriptor");
        goto clienterr;
    }

    len = sizeof(ss);
    if(getsockname(recvfd, (SA *) &ss, &len) < 0){
        err_ret("getsockname error");
        goto clienterr;
    }

    client[i].family = ss.ss_family;
    if ( (client[i].lport = sock_get_port((SA *) &ss, len)) == 0){
        client[i].lport = Sock_bind_wild(recvfd, client[i].family);
        if (client[i].lport <= 0){
            err_ret("error binding ephhemeral port");
            goto clienterr;
        }
    }
    write(unixfd, "1", 1);
    close(recvfd);
    return (--nready);

clienterr:
    Write(unixfd, "0", 1);
clientdone:
    Close(unixfd);
    if (recvfd >= 0)
        Close(recvfd);
    FD_CLR(unixfd, &allset);
    client[i].connfd = -1;
    return (--nready);
}


















ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd){
    struct msghdr msg;
    struct iovec iov[1];
    ssize_t n;

    union{
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr *cmptr;
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    
    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if ((n = recvmsg(fd, &msg, 0)) <= 0)
        return (n);
    

    if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL && cmptr->cmsg_len == CMSG_LEN(sizeof(int))){
        if (cmptr->cmsg_level != SOL_SOCKET)
            err_quit("control level != SOL_SOCKET");
        if (cmptr->cmsg_type != SCM_RIGHTS)
            err_quit("control type != SCM_RIGHTS");
        *recvfd = *((int *) CMSG_DATA(cmptr));
    }
    else{
        *recvfd = -1;
    }
    return n;
}