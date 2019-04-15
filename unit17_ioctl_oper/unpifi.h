#include "../myunp.h"
#include <net/if.h>

#define IFI_NAME    16
#define IFI_HADDR   8

struct ifi_info{
    char ifi_name[IFI_NAME];    //接口名称
    short ifi_index;            //接口索引
    short ifi_mtu;              //mtu
    u_char ifi_haddr[IFI_HADDR];        //硬件地址
    u_short ifi_hlen;                   //硬件地址长度
    short ifi_flags;                //接口标志
    short ifi_myflags;              //
    struct sockaddr *ifi_addr;      //主地址
    struct sockaddr *ifi_brdaddr;   //广播地址
    struct sockaddr *ifi_dstaddr;   //ptp链路目的地址
    struct ifi_info *ifi_next;      //next_ifiinfo
};

#define IFI_ALIAS       1

struct ifi_info *get_ifi_info(int, int);
struct ifi_info *Get_ifi_info(int, int);
void    free_ifi_info(struct ifi_info *);

