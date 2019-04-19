#include "../myunp.h"

int main(int argc, char **argv){
    int sockfd;
    if (argc != 3)
        err_quit("usage: tcpsend01 <host> <port>");
    sockfd = tcp_connect(argv[1], argv[2]);

    write(sockfd, "123", 3);
    printf("wrote 3 bytes normal data\n");
    sleep(1);

    send(sockfd, "4", 1, MSG_OOB);      //发送带外数据
    printf("wrote 1 byte of OOB data\n");
    sleep(1);

    write(sockfd, "56", 2);
    printf("wrote 2 bytes of normal data\n");
    sleep(1);

    send(sockfd, "7", 1, MSG_OOB);
    printf("wrote 1 byte of OOB data\n");
    sleep(1);

    write(sockfd, "89", 2);
    printf("wrote 2 bytes of normal data\n");
    sleep(1);

    exit(0);
}