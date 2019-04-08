#include "myunp.h"

void *
Calloc(size_t n, size_t size)
{
	void	*ptr;

	if ( (ptr = calloc(n, size)) == NULL)
		err_sys("calloc error");
	return(ptr);
}

void
Close(int fd)
{
	if (close(fd) == -1)
		err_sys("close error");
}

void
Dup2(int fd1, int fd2)
{
	if (dup2(fd1, fd2) == -1)
		err_sys("dup2 error");
}

int
Fcntl(int fd, int cmd, int arg)
{
	int	n;

	if ( (n = fcntl(fd, cmd, arg)) == -1)
		err_sys("fcntl error");
	return(n);
}

void
Gettimeofday(struct timeval *tv, void *foo)
{
	if (gettimeofday(tv, foo) == -1)
		err_sys("gettimeofday error");
	return;
}

int
Ioctl(int fd, int request, void *arg)
{
	int		n;
	if ( (n = ioctl(fd, request, arg)) == -1)
		err_sys("ioctl error");
	return(n);	/* streamio of I_LIST returns value */
}

pid_t
Fork(void)
{
	pid_t	pid;

	if ( (pid = fork()) == -1)
		err_sys("fork error");
	return(pid);
}

void *
Malloc(size_t size)
{
	void	*ptr;

	if ( (ptr = malloc(size)) == NULL)
		err_sys("malloc error");
	return(ptr);
}

int
Mkstemp(char *template)
{
	int i;

#ifdef HAVE_MKSTEMP
	if ((i = mkstemp(template)) < 0)
		err_quit("mkstemp error");
#else
	if (mktemp(template) == NULL || template[0] == 0)
		err_quit("mktemp error");
	i = Open(template, O_CREAT | O_WRONLY, FILE_MODE);
#endif

	return i;
}

#include	<sys/mman.h>

void *
Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	void	*ptr;

	if ( (ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *) -1))
		err_sys("mmap error");
	return(ptr);
}

int
Open(const char *pathname, int oflag, mode_t mode)
{
	int		fd;

	if ( (fd = open(pathname, oflag, mode)) == -1)
		err_sys("open error for %s", pathname);
	return(fd);
}

void
Pipe(int *fds)
{
	if (pipe(fds) < 0)
		err_sys("pipe error");
}

ssize_t
Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t		n;

	if ( (n = read(fd, ptr, nbytes)) == -1)
		err_sys("read error");
	return(n);
}

void
Sigaddset(sigset_t *set, int signo)
{
	if (sigaddset(set, signo) == -1)
		err_sys("sigaddset error");
}

void
Sigdelset(sigset_t *set, int signo)
{
	if (sigdelset(set, signo) == -1)
		err_sys("sigdelset error");
}

void
Sigemptyset(sigset_t *set)
{
	if (sigemptyset(set) == -1)
		err_sys("sigemptyset error");
}

void
Sigfillset(sigset_t *set)
{
	if (sigfillset(set) == -1)
		err_sys("sigfillset error");
}

int
Sigismember(const sigset_t *set, int signo)
{
	int		n;

	if ( (n = sigismember(set, signo)) == -1)
		err_sys("sigismember error");
	return(n);
}

void
Sigpending(sigset_t *set)
{
	if (sigpending(set) == -1)
		err_sys("sigpending error");
}

void
Sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
	if (sigprocmask(how, set, oset) == -1)
		err_sys("sigprocmask error");
}

char *
Strdup(const char *str)
{
	char	*ptr;

	if ( (ptr = strdup(str)) == NULL)
		err_sys("strdup error");
	return(ptr);
}

long
Sysconf(int name)
{
	long	val;

	errno = 0;		/* in case sysconf() does not change this */
	if ( (val = sysconf(name)) == -1)
		err_sys("sysconf error");
	return(val);
}

#ifdef	HAVE_SYS_SYSCTL_H
void
Sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp,
	   void *newp, size_t newlen)
{
	if (sysctl(name, namelen, oldp, oldlenp, newp, newlen) == -1)
		err_sys("sysctl error");
}
#endif

void
Unlink(const char *pathname)
{
	if (unlink(pathname) == -1)
		err_sys("unlink error for %s", pathname);
}

pid_t
Wait(int *iptr)
{
	pid_t	pid;

	if ( (pid = wait(iptr)) == -1)
		err_sys("wait error");
	return(pid);
}

pid_t
Waitpid(pid_t pid, int *iptr, int options)
{
	pid_t	retpid;

	if ( (retpid = waitpid(pid, iptr, options)) == -1)
		err_sys("waitpid error");
	return(retpid);
}

void
Write(int fd, void *ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
		err_sys("write error");
}

static void	err_doit(int, const char *, va_list);

char	*pname = NULL;		/* caller can set this from argv[0] */

/* Nonfatal error related to a system call.
 * Print a message and return. */

void
/* $f err_ret $ */
err_ret(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error related to a system call.
 * Print a message and terminate. */

void
/* $f err_sys $ */
err_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	exit(1);
}

/* Fatal error related to a system call.
 * Print a message, dump core, and terminate. */

void
/* $f err_dump $ */
err_dump(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, fmt, ap);
	va_end(ap);
	abort();		/* dump core and terminate */
	exit(1);		/* shouldn't get here */
}

/* Nonfatal error unrelated to a system call.
 * Print a message and return. */

void
/* $f err_msg $ */
err_msg(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error unrelated to a system call.
 * Print a message and terminate. */

void
/* $f err_quit $ */
err_quit(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(1);
}

/* Print a message and return to caller.
 * Caller specifies "errnoflag". */

static void
err_doit(int errnoflag, const char *fmt, va_list ap)
{
	int		errno_save;
	char	buf[MAXLINE];

	errno_save = errno;		/* value caller might want printed */
	vsprintf(buf, fmt, ap);
	if (errnoflag)
		sprintf(buf+strlen(buf), ": %s", strerror(errno_save));
	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(stderr);		/* SunOS 4.1.* doesn't grok NULL argument */
	return;
}

const char *
Inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	const char	*ptr;

	if (strptr == NULL)		/* check for old code */
		err_quit("NULL 3rd argument to inet_ntop");
	if ( (ptr = inet_ntop(family, addrptr, strptr, len)) == NULL)
		err_sys("inet_ntop error");		/* sets errno */
	return(ptr);
}

void
Inet_pton(int family, const char *strptr, void *addrptr)
{
	int		n;

	if ( (n = inet_pton(family, strptr, addrptr)) < 0)
		err_sys("inet_pton error for %s", strptr);	/* errno set */
	else if (n == 0)
		err_quit("inet_pton error for %s", strptr);	/* errno not set */

	/* nothing to return */
}

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t my_read(int fd, char *ptr){
	if (read_cnt <= 0){
		again:
			if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0){
				if (errno = EINTR)
					goto again;
				return -1;
			}
			else if (read_cnt == 0)
				return 0;
			read_ptr = read_buf;
	}

	read_cnt --;
	*ptr = *read_ptr++;
	return 1;
}

ssize_t readline(int fd, void *vptr, size_t maxlen){
	char c, *ptr;
	int n, rc;
	ptr = vptr;
	for (n = 1; n < maxlen; n++){
		if ((rc = my_read(fd, &c)) == 1){
			*ptr ++ = c;
			if (c == '\n')
				break;
		}
		else if (rc == 0){
			*ptr = 0;
			return n-1;
		}
		else
			return -1;		
	}
	*ptr = 0;
	return n;
}

ssize_t readlinebuf(void **ptr){
	if (read_cnt){
		*ptr = read_ptr;
	}
	return read_cnt;
}

ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}
/* end writen */

void
Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		err_sys("writen error");
}

char *sock_ntop(const struct sockaddr *sa, socklen_t salen){
	char portstr[8];
	static char str[128];
	switch(sa->sa_family){
		case AF_INET:{
			struct sockaddr_in *sin = (struct sockaddr_in *) sa;
			if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
				return NULL;
			if (ntohs(sin->sin_port) != 0){
				snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
				strcat(str, portstr);
			}
			return str;
		}
		case AF_INET6: {
			struct sockaddr_in6 *sin6 = (struct sockaddt_in6 *) sa;
			str[0] = '[';
			if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
				return NULL;	
			if (ntohs(sin6->sin6_port) != 0){
				snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
				strcat(str, portstr);
			}		
			return str;
		}
	#ifdef	AF_UNIX
	case AF_UNIX: {
		struct sockaddr_un	*unp = (struct sockaddr_un *) sa;

			/* OK to have no pathname bound to the socket: happens on
			   every connect() unless client calls bind() first. */
		if (unp->sun_path[0] == 0)
			strcpy(str, "(no pathname bound)");
		else
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		return(str);
	}
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
	case AF_LINK: {
		struct sockaddr_dl	*sdl = (struct sockaddr_dl *) sa;

		if (sdl->sdl_nlen > 0)
			snprintf(str, sizeof(str), "%*s (index %d)",
					 sdl->sdl_nlen, &sdl->sdl_data[0], sdl->sdl_index);
		else
			snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
		return(str);
	}
#endif
	default:
		snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d",
				 sa->sa_family, salen);
		return(str);	
	}
	return NULL;
}


const char *
hstrerror(int err)
{
	if (err == 0)
		return("no error");

	if (err == HOST_NOT_FOUND)
		return("Unknown host");

	if (err == TRY_AGAIN)
		return("Hostname lookup failure");

	if (err == NO_RECOVERY)
		return("Unknown server error");

	if (err == NO_DATA)
        return("No address associated with name");

	return("unknown error");
}
