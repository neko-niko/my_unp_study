#include <event.h>
#include "../myunp.h"
void str_cli(FILE *fp, int sockfd){
    int kq, i, n, nev, stdineof = 0, isfile;
    char buf[100];
    struct kevent kev[2];
    struct timespec ts;
    struct stat st;

    isfile = ((fstat(fileno(fp), &st) == 0) && (st.st_mode & __S_IFMT) == __S_IFREG);

    EV_SET(&kev[0], fileno(fp), EVFILT_READ, EV_ADD, 0, 0, NULL);
    EV_SET(&kev[1], sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kq = Kqueue();
    ts.tv_sec = ts.tv_nsec = 0;
    kevent(kq, kev, 2, NULL, 0, &ts);

    while (1){
        nev = kevent(kq, NULL, 0, kev, 2, NULL);

        for (i = 0; i < nev; i++){
            if (kev[i].ident == sockfd){
                if ((n = raed(sockfd, buf, MAXLINE)) == 0){
                    if (stdineof == 1)
                        return ;
                    else
                        err_quit("str_cli: server terminated prematurely");
                }
                write(fileno(stdout), buf, n);
            }

            if (kev[i].ident == fileno(fp)){
                n = read(fileno(fp), buf, MAXLINE);
                if (n > 0)
                    Writen(sockfd, buf, n);
                if (n == 0 || (isfile && n == kev[i].data)){
                    stdineof = 1;
                    shutdown(sockfd, SHUT_WR);
                    kev[i].flags = EV_DELETE;
                    kevent(kq, &kev[i], 1, NULL, 0, &ts);
                    continue;
                }
            }
        }
    }
}
