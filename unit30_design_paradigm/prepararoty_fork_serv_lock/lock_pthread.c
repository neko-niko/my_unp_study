#include "../unpthread.h"
#include <sys/mman.h>

static pthread_mutex_t *mptr;

void my_lock_init(char *pathname){
    int fd;
    pthread_mutexattr_t mattr;

    fd = open("/dev/zero", O_RDWR, 0);

    mptr = Mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    Pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setprioceiling(&mattr, PTHREAD_PROCESS_SHARED);
    Pthread_mutex_init(mptr, &mattr);
}

void my_lock_wait(){
    Pthread_mutex_lock(mptr);
}

void my_lock_release(){
    Pthread_mutex_unlock(mptr);
}

