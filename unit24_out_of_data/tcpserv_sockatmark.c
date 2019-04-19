#include "../myunp.h"

int main(int argc, char **argv){
    int listenfd, connfd, n, on = 1;
    char buf[100];

    if (argc == 2)
        listenfd = tcp_listen(NULL, argv[1], NULL);
    else if (argc == 3)
        listenfd = tcp_listen(argv[1], argv[2], NULL);
    else
        err_quit("usage: tcprecv_sockatmark [ <host> ] <port>");

    setsockopt(listenfd, SOL_SOCKET, SO_OOBINLINE, &on, sizeof(on));

    connfd = accept(listenfd, NULL, NULL);
    sleep(5);

    while (1){
        if (sockatmark(connfd))
            printf("at OOB mark\n");
        
        if ((n = read(connfd, buf, sizeof(buf) - 1)) == 0){
            print("recvived EOF\n");
            exit(0);
        }

        buf[n] = 0;
        printf("read %d bytes: %s\n", n, buf);
        
    }
}