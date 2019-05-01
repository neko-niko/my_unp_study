#include "../myunp.h"



typedef struct {
    pid_t child_pid;
    int child_pipefd;
    int child_status;
    long child_count;       //子进程已处理客户的计数

} Child;

Child *cptr;
