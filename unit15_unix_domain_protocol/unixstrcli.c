#include "../myunp.h"

int main(int argc, char *argv){
    int sockfd;
    struct sockaddr_un cliaddr, servaddr;
    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

    bzero(&cliaddr, sizeof(struct sockaddr_un));
    cliaddr.sun_family = AF_LOCAL;
    strcpy(cliaddr.sun_path, UNIXSTR_PATH);
    connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
    char sendline[MAXLINE], recvline[MAXLINE];
    while (fgets(sendline, MAXLINE, stdin) != NULL){
        write(sockfd, sendline, strlen(sendline));
        if (readline(sockfd, recvline, MAXLINE) == 0){
            err_quit("str_cli: server terminated prematurely");
        }
        fputs(recvline, stdout);
    }
}