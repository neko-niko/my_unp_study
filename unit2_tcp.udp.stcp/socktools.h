#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <sys/un.h>

#define bzero(ptr, n)   memset(ptr, 0, n)
typedef struct sockaddr SA;

char* sock_ntop(const struct sockaddr *sa, socklen_t salen);

int sock_bind_wild(int sockfd, int family);
int sock_cmp_addr(const struct sockaddr *sockaddr1, const struct sockaddr *sockaddr2, socklen_t addrlen);
int sock_cmp_port(const struct sockaddr *sockaddr1, const struct sockaddr *sockaddr2, socklen_t addrlen);
char *sock_ntop_host(const struct sockaddr *sockaddr, socklen_t addrlen);
void sock_set_addr(const struct sockaddr *sockaddr, socklen_t addrlen, void *ptr);
void sock_set_port(const struct sockaddr *sockaddr, socklen_t addrlen, int ptr);
void sock_set_wild(struct sockaddr *sockaddr, socklen_t addrlen);
