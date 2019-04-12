#include "../myunp.h"

int main(int argc, char **argv){
    int sockfd;
    struct sockaddr_un servaddr, cliaddr;
    sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    unlink(UNIXDG_PATH);
    bzero(&servaddr, sizeof(struct sockaddr_un));

    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXDG_PATH);
    bind(sockfd, (SA *)&servaddr, sizeof(servaddr));
    
    char recvline[MAXLINE];
    int n;
    socklen_t len;

    while (1){
        len = sizeof(struct sockaddr_un);
        n = recvfrom(sockfd, recvline[MAXLINE], MAXLINE, 0, &cliaddr, &len);
        sendto(sockfd, recvline, n, 0, &cliaddr, sizeof(cliaddr));
    }
}