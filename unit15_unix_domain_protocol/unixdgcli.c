#include "../myunp.h"

int main(int argc, char **argv){
    int sockfd;
    struct sockaddr_un cliaddr, servaddr;
    sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    bzero(&cliaddr, sizeof(cliaddr));

    cliaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, tmpnam(NULL));
    bind(sockfd, (SA *) &cliaddr, sizeof(cliaddr));

    bzero(&servaddr, sizeof(servaddr));
    strcpy(servaddr.sun_path, UNIXSTR_PATH);
    servaddr.sun_family = AF_LOCAL;

    char recvline[MAXLINE], sendline[MAXLINE];
    int n;
    socklen_t len;
    while(fgets(sendline, MAXLINE, stdin) != NULL){
        sendto(sockfd, sendline, MAXLINE, 0, &servaddr, sizeof(servaddr));
        len = sizeof(servaddr);
        n = recvfrom(sockfd, recvline, MAXLINE, 0, (SA *) &servaddr, &len);
        if (len != sizeof(servaddr)){
            continue;
        }
        recvline[n] = 0;
        fputs(recvline, stdout);
    }
}