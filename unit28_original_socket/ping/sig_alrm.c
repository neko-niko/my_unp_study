#include "ping.h"

void sig_alrm(int singo){
    (*pr->fsend)();
    alarm(1);
    return ;
}


