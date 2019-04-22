#include "../unpthread.h"
#include <pthread.h>

#define MAXFILES    20
#define SERV        "80"

struct file{
    char *f_name;
    char *f_host;
    int f_fd;
    int f_flags;
    pthread_t f_tid;
} file[MAXLINE];

#define F_CONNECTING    1
#define F_READING       2
#define F_DONE          4

#define GET_CMD     "GET %s HTTP/1.0\r\n\r\n"

int nconn, nfiles, nlefttoconn, nlefttoread;
void *do_get_read(void *);
void home_page(const char *, const char *);
void write_get_cmd(struct file *);

int main(int argc, char **argv){
    int i, n, maxnconn;
    pthread_t tid;
    struct file *fptr;

    if (argc < 5)
        err_quit("usage: web <#conns> <IPsec> <homepage> file1 ...");
    maxnconn = atoi(argv[1]);

    nfiles = min(argc - 4, MAXFILES);
    for (i = 0; i < nfiles; i++){
        file[i].f_name = argv[i+4];
        file[i].f_host = argv[2];
        file[i].f_flags = 0;
    }

    printf("nfiles = %d\n", nfiles);

    home_page(argv[2], argv[3]);

    nlefttoread = nlefttoconn = nfiles;
    nconn = 0;

    while (nlefttoconn > 0){
        while (nconn < maxnconn && nlefttoconn > 0){
            /* find a file to read */
            for(i = 0; i < nfiles; i++)
                if (file[i].f_flags == 0)
                    break;
            if (i == nfiles)
                err_quit("nlefttoconn = %d but nothing found", nlefttoconn);
            file[i].f_flags = F_CONNECTING;
            Pthread_create(&tid, NULL, &do_get_read, &file[i]);
            file[i].f_tid = tid;
            nconn ++;
            nlefttoconn --;
        }

         //使用Solaris线程函数（linux无法使用）等待任何一个线程终止， POSIX标准无法做到这一点（需要借助条件变量）
        if ((n = thr_join(0, &tid, (void **) &fptr)) != 0){     
            errno = n, err_sys("thr_join error");
        }     

        nconn --;
        nlefttoread --;
        printf("thread id %d for %s done\n", tid, fptr->f_name);
    }
    exit(0);
}

void home_page(const char *host, const char *fname){
    int fd, n;
    char line[MAXLINE];
    
    fd = tcp_connect(host, SERV);
    n = snprintf(line, sizeof(line), GET_CMD, fname);
    Writen(fd, line, n);
    while (1){
        if ((n = read(fd, line, MAXLINE)) == 0)
            break;
        printf("read %d bytes of home page\n", n);
    }
    printf("end-of-file on home page\n");
    close(fd);
}

void *do_get_read(void *vptr){
    int fd, n;
    char line[MAXLINE];
    struct file *fptr;

    fptr = (struct file *) vptr;

    fd = tcp_connect(fptr->f_host, SERV);
    fptr->f_fd = fd;
    printf("do_get_read for %s, fd %d, thread %d\n", fptr->f_name, fd, fptr->f_tid);

    write_get_cmd(fptr);

    while(1){
        if ((n = read(fd, line, MAXLINE)) == 0)
            break;
        printf("read %d bytes from %s\n", n, fptr->f_name);
        close(fd);
        fptr->f_flags = F_DONE;
        return fptr;
    }
}


void write_get_cmd(struct file *fptr){
    int n;
    char line[MAXLINE];

    n = snprintf(line, sizeof(line), GET_CMD, fptr->f_name);
    Writen(fptr->f_fd, line, n);
    printf("wrote %d bytes for %s\n", n, fptr->f_name);

    fptr->f_flags = F_READING;
}