#include "unpicmpd.h"

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen){
    int icmpfd, maxfdp1;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    fd_set rset;
    ssize_t n;
    struct timeval tv;
    struct icmpd_err icmpd_err;
    struct sockaddr_un sun;

    Sock_bind_wild(sockfd, pservaddr->sa_family);

    icmpfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    sun.sun_family = AF_LOCAL;
    strcpy(sun.sun_path, ICMPD_PATH);
    connect(icmpfd, (SA *) &sun, sizeof(sun));
    write_fd(icmpfd, "1", 1, sockfd);
    n = read(icmpfd, recvline, 1);
    if (n != 1 || recvline[0] != '1')
        err_quit("error creating icmp socket, n = %d, char = %c", n, recvline[0]);
    FD_ZERO(&rset);
    maxfdp1 = max(sockfd, icmpfd) + 1;
    
    while (fgets(sendline, MAXLINE, fp) != NULL){
        sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        tv.tv_sec = 5;
        tv.tv_usec = 0;
        FD_SET(sockfd, &rset);
        FD_SET(sockfd, &rset);
        if ((n = select(maxfdp1, &rset, NULL, NULL, &tv)) == 0){
            fprintf(stderr, "socket timeout\n");
            continue;
        }

        if (FD_ISSET(sockfd, &rset)){
            n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
            recvline[n] = 0;
            fputs(recvline, stdout);
        }

        if (FD_ISSET(icmpfd, &rset)){
            if ((n = read(icmpfd, &icmpd_err, sizeof(icmpd_err))) == 0)
                err_quit("ICMP deamon terminated");
            else if (n != sizeof(icmpd_err))
                err_quit("n = %d, expected %d", n, sizeof(icmpd_err));
            printf("ICMP error: dest = %s, %s, type = %d, code = %d\n", sock_ntop(&icmpd_err.icmpd_dest, icmpd_err.icmpd_len), 
            strerror(icmpd_err.icmpd_errno), icmpd_err.icmpd_type, icmpd_err.icmpd_code);
        }
    }
}


ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd){
    struct msghdr msg;
    struct iovec iov[1];

    union{
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    } control_un;

    struct cmsghdr *cmptr;
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len = CMSG_LEN(sizeof(int));
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    *((int *) CMSG_DATA(cmptr)) = sendfd;
    
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    return (sendmsg(fd, &msg, 0));
}