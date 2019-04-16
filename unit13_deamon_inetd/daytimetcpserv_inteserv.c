#include "../myunp.h"
#include <syslog.h>
#include <time.h>

extern int daemon_proc;

void daemon_inetd(const char *pname, int facility){
    daemon_proc = 1;
    openlog(pname, LOG_PID, facility);

}

int main(int argc, char **argv){
    socklen_t len;
    struct sockaddr *cliaddr;
    char buff[MAXLINE];
    time_t ticks;

    daemon_inetd(argv[0], 0);
    cliaddr = Malloc(sizeof(struct sockaddr_storage));
    len = sizeof(struct sockaddr_storage);
    getpeername(0, cliaddr, &len);      //  已在inet父进程中将描述符重定向到socketfd
    err_msg("connection from %s", sock_ntop(cliaddr, len));
    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(ticks));
    Write(0, buff, strlen(buff));
    exit(0);
}