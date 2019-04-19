#include <sys/param.h>
#include "../myunp.h"

//在recvfrom的基础上还返回
//1.所返回msg_flags
//2.数据报的目的地址（通过IP_RECVDSTADDR获取)
//3.所收取数据报接受借口的索引(通过IP_RECVIF套接字选项)

ssize_t recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp, SA *sa, socklen_t *salenptr, struct unp_in_pktinfo *pktp){
    struct msghdr msg;
    struct iovec iov[1];
    ssize_t n;

    struct cmsghdr *cmptr;
    union {
        char control[CMSG_SPACE(sizeof(struct in_addr)) + CMSG_SPACE(sizeof(struct unp_in_pktinfo))];
        struct cmsghdr cm;
    }control_un;
    
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    msg.msg_flags = 0;


    msg.msg_name = sa;
    msg.msg_namelen = *salenptr;
    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if ((n = recvmsg(fd, &msg, &flagsp)) < 0)
        return n;
    
    *salenptr = msg.msg_namelen;
    if (pktp){
        bzero(pktp, sizeof(struct unp_in_pktinfo));
    }

    //处理得到的msg信息

    *flagsp = msg.msg_flags;        //返回msg_flag的值， 如果1.没有控制信息 2.控制信息被截断 3.调用者不指定pktp 则返回
    if (msg.msg_controllen < sizeof(struct cmsghdr) || (msg.msg_flags & MSG_CTRUNC) || pktp == NULL){
        return n;
    }

    //处理辅助数据
    for (cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL, cmptr = CMSG_NXTHDR(&msg, cmptr)){
        //如果目的ip地址作为控制信息返回， 则返回给调用者
#ifdef IP_RECVDSTADDR
        if (cmptr->cmsg_level == IPPROTO_IP && cmptr->cmsg_type == IP_RECVDSTADDR){
            memcpy(&pktp->ipi_addr, CMSG_DATA(cmptr), sizeof(struct in_addr));
            continue;
        }
#endif

        //如果接受接口的索引作为控制信息返回， 则把他返还给调用者
#ifdef IP_RECVIF
        if (cmptr->cmsg_level == IPPROTO_IP && cmptr->cmsg_type == IP_RECVIF){
            struct sockaddr_dl *sdl;
            sdl = (struct sockaddr_dl *)    CMSG_DATA(cmptr);
            pktp->ipi_ifindex = sdl->sdl_index;
            continue;
        }
#endif
        err_quit("unkown ancillary data, len = %d level = %d type = %d", cmptr->cmsg_len, cmptr->cmsg_level, cmptr->cmsg_type);
    }
    return n;
}