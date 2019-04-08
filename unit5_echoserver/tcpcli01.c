#include "../myunp.h"

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
    char recvline[MAXLINE], sendline[MAXLINE];
    while (fgets(sendline, MAXLINE, fp) != NULL){
        Write(sockfd, sendline, strlen(sendline));

        if (readline(sockfd, recvline, MAXLINE) == 0){
            err_quit("str_cli: server terminated prematurely");
        }
        fputs(recvline, stdout);
    }
}
