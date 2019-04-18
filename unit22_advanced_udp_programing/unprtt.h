#include "../myunp.h"

struct rtt_info{
    float rtt_rtt;      //最近测量的RTT
    float rtt_srtt;     //平滑化RTT估算因子
    float rtt_rttvar;   //平滑化平均偏差估算因子
    float rtt_rto;      //当前使用的rto
    int rtt_nrexmt;     //重传次数
    uint32_t rtt_base;  // 1/1/1970
};

#define RTT_RXTMIN      2
#define RTT_RXTMAX      60
#define RTT_MAXNREXMT   3


void rtt_debug(struct rtt_info *);
void rtt_init(struct rtt_info *);
void rtt_newpack(struct rtt_info *);
int rtt_start(struct rtt_info *);
void rtt_stop(struct rtt_info *, uint32_t);
int rtt_timeout(struct rtt_info *);
uint32_t rtt_ts(struct rtt_info *);

extern int rtt_d_flag;

