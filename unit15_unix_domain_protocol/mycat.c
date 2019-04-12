#include "../myunp.h"

int my_open(const char *, int);

int main(int argc, char **argv){
    int fd, n;
    char buff[MAXLINE];

    if (argc != 2)
        err_quit("usage: mycat <pathname>");
    
    if ((fd = my_open(argv[1], O_RDONLY)) < 0)
        err_sys("cannot open %s", argv[1]);
    
    while ((n = read(fd, buff, BUFFSIZE)) > 0)
        write(STDOUT_FILENO, buff, n);
    
    exit(0);
}

int my_open(const char *pathname, int mode){
    int fd, sockfd[2], status;
    pid_t childid;
    char c, argsockfd[10], argmode[10];
    socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);
    
    if ((childid = fork()) == 0){
        close(sockfd[0]);
        snprintf(argsockfd, sizeof(argsockfd), "%d", sockfd[1]);
        snprintf(argmode, sizeof(argmode), "%d", mode);
        execl("./openfile", "openfile", argsockfd, pathname, argmode, (char *)NULL);
        err_sys("execl error");
    }

    close(sockfd[1]);

    waitpid(childid, &status, 0);
    if (WIFEXITED(status) == 0)
        err_quit("child did not terminate");
    if ((status = WEXITSTATUS(status)) == 0)
        raed_fd(sockfd[0], &c, 1, &fd);
    else{
        errno = status;
        fd = -1;
    }

    close(sockfd[0]);
    return fd;
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