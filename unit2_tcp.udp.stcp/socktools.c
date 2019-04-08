#include "socktools.h"

char* sock_ntop(const struct sockaddr *sa, socklen_t salen){
    char portstr[8];
    char str[128];
    switch (sa->sa_family){
        case AF_INET: {
            struct sockaddr_in *sin = (struct sockaddr_in *) sa;
            if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
                return NULL;
            if (ntohs(sin->sin_port) != 0){
                snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
                strcat(str, portstr);
            }
            return (str);
        }
    }
}

int sock_bind_wild(int sockfd, int family){     //将通配地址和一个临时端口捆绑到一个socket
    socklen_t len;
    switch(family){
        case AF_INET:{
            struct sockaddr_in sin;
            
            bzero(&sin, sizeof(sin));
            sin.sin_family = AF_INET;
            sin.sin_addr.s_addr = htonl(INADDR_ANY);
            sin.sin_port = htons(0);

            if (bind(sockfd, (SA *) &sin, sizeof(sin)) < 0)
                return (-1);
            len = sizeof(sin);
            if (getsockname(sockfd, (SA *)&sin, &len) < 0)
                return (-1);
            return (sin.sin_port);
        }

        case AF_INET6: {
            struct sockaddr_in6 sin6;
            bzero(&sin6, sizeof(sin6));
            sin6.sin6_addr = in6addr_any;
            sin6.sin6_port = htons(0);
            sin6.sin6_family  = AF_INET6;

            if (bind(sockfd, (SA*)&sin6, sizeof(sin6)) < 0)
                return (-1);
            len = sizeof(sin6);
            if (getsockname(sockfd, (SA *) &sin6, &len) < 0)
                return (-1);
            return (sin6.sin6_port);
        }
    }
    return (-1);
}

int sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t addrlen){
    /*比较两个套件字地址结构的地址部分*/
    if (sa1->sa_family != sa2->sa_family)
        return (-1);
    switch(sa1->sa_family){
        case AF_INET:
            return (memcmp( &((struct sockaddr_in *) sa1)->sin_addr, &((struct sockaddr_in *) sa2)->sin_addr, sizeof(struct in_addr)));
        case AF_INET6:
            return (memcmp( &((struct sockaddr_in6 *) sa1)->sin6_addr, &((struct sockaddr_in6 *) sa2)->sin6_addr, sizeof(struct in6_addr)));
        case AF_UNIX:
            return (strcmp( ((struct sockaddr_un *)sa1)->sun_path, 
                            ((struct sockaddr_un *)sa2)->sun_path));
    }
    return (-1);
}