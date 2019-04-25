#include "../../myunp.h"

#define ICMPD_PATH      "/tmp/icmpd"        //unix域套件字路径名

struct icmpd_err{       //传递给应用进程的icmpd_err结构
    int icmpd_errno;
    char icmpd_type;
    char icmpd_code;
    socklen_t icmpd_len;
    struct sockaddr_storage icmpd_dest;     //存放引发本icmp错误的数据包IP地址和目的端口
};



