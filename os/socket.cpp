#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#ifndef WIN32
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <sys/utsname.h>
	#include <unistd.h>
#else
//	#include <io.h>
#endif

#ifdef WIN32
		#include <winsock2.h>
		#include <ws2tcpip.h>
#endif

#include "osmacros.h"
#include "str.h"
#include "assert.h"
#include "socket.h"
#include "protected.h"

//static Mutex *inet_mutex;

//static Octstr *official_name = NULL;
static char g_strHostName[80];
static char g_strHostIP[20];

static struct hostent localhostinfo;

// Multicast address and other information
static char g_sMulticastAddress[16];	// The mulicast address
static int g_nMultiCastPort = 2380;

// reference count
static int iInitilizated = 0;

/*
// Is this string is an ip address?
#define IsIP(x) \
	do { \
	char *p = (x); \
	for(; p && *p ; p++) \
	{ \
		if (!isdigit(*p) && *p != '.') \
		{ \
			printf("Error ip address:%s\n", (x)); \
			return -1; \
		} \
	} while(0) 
*/
static int IsIP(char *ip)
{
	char *p = ip;

	// any address
	if (strcmp(ip, "*") ==0)
		return 1;

	for( ; p && *p ; p++)
	{
		if (!isdigit(*p) && *p != '.')
		{
			printf("Error ip address:%s\n", ip);
			return 0;
		}
	}

	return 1;
}

int OSMakeServerTCPSocket(int port)
{

    struct sockaddr_in addr;
    int s;
    int reuse;

    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1) 
	{
#ifdef WIN32
        printf("socket failed, error code:%d\n", WSAGetLastError());
#else
        printf("socket failed, error code:%d\n", errno);
#endif
        goto error;
    }
	printf("=>socket created %d\n", s);
	memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

	addr.sin_port = htons(port);

    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    reuse = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) == -1) 
	{
        printf("setsockopt failed for server address");
        goto error;
    }

    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) == -1) 
	{
        printf("bind failed, error %d\n", errno);
        goto error;
    }

    if (listen(s, 10) == -1) 
	{
        printf("listen failed");
        goto error;
    }

    return s;

error:
    if (s >= 0)
#ifdef WIN32
		(void) closesocket(s);
#else
        (void) close(s);
#endif
    return -1;
}

int OSTCPConnectWithName(char *hostname, int port)
{

    return OSTCPConnectWithNamePort(hostname, port, 0);
}

/**
 * Connect to server with ip instead of using hostname.
 */
int OSTCPConnectWithIP(char *ip, int port)
{

    return OSTCPConnectWithIPPort(ip, port, 0);
}


/*
 * Prevent using gethostbyname, when I have an ip address.
 */
int OSTCPConnectWithIPPort(char *ip, int port, int our_port)
{
    struct sockaddr_in addr;
    struct sockaddr_in o_addr;
    int s;

	RETVIF(ip, -1);

    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        printf("Couldn't create new socket.\n");
        goto error;
    }

    addr.sin_family = AF_INET;
    addr.sin_port	= htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (our_port > 0) {
        int reuse;

        o_addr.sin_family = AF_INET;
        o_addr.sin_port = htons(our_port);
        o_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        reuse = 1;
        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse,
                       sizeof(reuse)) == -1) {
            printf("setsockopt failed before bind");
            goto error;
        }
        if (bind(s, (struct sockaddr *) &o_addr, sizeof(o_addr)) == -1) {
            printf("bind to local port %d failed", our_port);
            goto error;
        }
    }

    if (connect(s, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        printf("connect failed");
        goto error;
    }

    return s;

error:
    error(0, "error connecting to server `%s' at port `%d'", ip, port);
    if (s >= 0)
#ifdef WIN32
		closesocket(s);
#else
        close(s);
#endif
    return -1;
}

static int OSGetHostByName(struct hostent *ent, const char *name) 
{
	int ret = -1;
	struct hostent *p;
	
//	lock(GETHOSTBYNAME);
	p = gethostbyname(name);
	if (p == NULL)
		ret = -1;
	else {
		ret = 0;
		*ent = *p;
	}
//	unlock(GETHOSTBYNAME);
	return ret;
}

int OSTCPConnectWithNamePort(char *hostname, int port, int our_port)
{
    struct sockaddr_in addr;
    struct sockaddr_in o_addr;
    struct hostent hostinfo;
    int s;

    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        printf("Couldn't create new socket.\n");
        goto error;
    }

    if (OSGetHostByName(&hostinfo, hostname) == -1) {
        printf("gethostbyname failed");
        goto error;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *(struct in_addr *) hostinfo.h_addr;

    if (our_port > 0) {
        int reuse;

        o_addr.sin_family = AF_INET;
        o_addr.sin_port = htons(our_port);
        o_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        reuse = 1;
        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse,
                       sizeof(reuse)) == -1) {
            printf("setsockopt failed before bind");
            goto error;
        }
        if (bind(s, (struct sockaddr *) &o_addr, sizeof(o_addr)) == -1) {
            printf("bind to local port %d failed", our_port);
            goto error;
        }
    }

    if (connect(s, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        printf("connect failed");
        goto error;
    }

    return s;

error:
    printf("error connecting to server `%s' at port `%d'", hostname, port);
    if (s >= 0)
#ifdef WIN32
		closesocket(s);
#else
        close(s);
#endif
    return -1;
}

/**
 * Write string to socket. 
 * Return values:
 *			-1 : error
 *			 0 : success
 *			+0 : bytes written
 */
int OSWriteToSocket(int socket, char *str)
{
    size_t len;
    int ret;
 
    len = strlen(str);
    while (len > 0) 
	{
        ret = send(socket, str, len, 0);
        if (ret == SOCKET_ERROR) {
            if (errno == EAGAIN) continue;
            if (errno == EINTR) continue;
            printf("Writing to socket failed");
            return -1;
        }
        /* ret may be less than len, if the writing was interrupted
           by a signal. */
        len -= ret;
        str += ret;
    }

    return len;
}

/**
 * Write a binary buffer to socket.
 * Return -1 for error, 0 for success, >0 for bytes sent
 */
int OSTCPSend(int socket, char *buf, int len)
{
    int ret;
 
    while (len > 0) 
	{
        ret = send(socket, buf, len, 0);
        if (ret == SOCKET_ERROR) 
		{
            if (errno == EAGAIN) continue;
            if (errno == EINTR) continue;
            printf("Writing to socket failed");
            return -1;
        }
        /* ret may be less than len, if the writing was interrupted
           by a signal. */
        len -= ret;
        buf += ret;
    }

    return len;
}

/**
 * Read a binary buffer from socket.
 * Return -1 for error, 0 for success, >0 for bytes got
 * nTimeOutMilliSeconds in millionseconds
 */
int OSTCPRecv(int socket, char *buf, int max, bool fill_up, long nTimeOutMilliSeconds)
{
    int ret, nGotBytes = 0;

	if (fill_up)
	{
		while (max > 0)
		{
			if (nTimeOutMilliSeconds > 0)
			{
				if (ret = OSReadAvailable(socket, nTimeOutMilliSeconds * 1000) < 1)
					return ret;
			}

			ret = recv(socket, buf, max, 0);
			if (ret == SOCKET_ERROR) 
			{
				//			if (errno == EAGAIN) continue;
				//			if (errno == EINTR) continue;
				printf("read failed with error:%s\n", strerror(errno));
				return -1;
			}

			if (ret == 0)
				break;

			buf += ret;
			max -= ret;
			nGotBytes += ret;
		}
	}
	else
	{
		if (nTimeOutMilliSeconds > 0)
		{
			if (ret = OSReadAvailable(socket, nTimeOutMilliSeconds * 1000) < 1)
				return ret;
		}

		ret = recv(socket, buf, max, 0);
		if (ret == SOCKET_ERROR) 
		{
			//			if (errno == EAGAIN) continue;
			//			if (errno == EINTR) continue;
			printf("read failed with error:%s\n", strerror(errno));
			return -1;
		}
		nGotBytes = ret;
	}
	return nGotBytes;
}

// Is this socket is blocking?
int OSIsSocketBlock(int fd)
{
#ifndef WIN32
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        printf("cannot tell if fd %d is blocking", fd);
        return -1;
    }

    return (flags & O_NONBLOCK) != 0;
#else
	return 0;
#endif
}

int OSSetBlockSocket(int fd, int blocking)
{
#ifndef WIN32
    int flags, newflags;

    flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        printf("cannot get flags for fd %d", fd);
        return -1;
    }

    if (blocking)
        newflags = flags & ~O_NONBLOCK;
    else
        newflags = flags | O_NONBLOCK;

    if (newflags != flags) {
        if (fcntl(fd, F_SETFL, newflags) < 0) {
            printf("cannot set flags for fd %d", fd);
            return -1;
        }
    }
#else
	blocking = 1 - blocking;

	if (ioctlsocket(fd, FIONBIO, (unsigned long *)&blocking) != 0)
	{
		printf("Cannot set blocking flag to this socket:%d, errno:%d.", fd, WSAGetLastError());
		return -1;
	}
#endif

	return 0;
}

// Get IP address from one given socket, and you should delete this char* result
char *OSSocketGetPeerIP(int socket)
{
    socklen_t len;
    struct sockaddr_in addr;

    len = sizeof(addr);
    if (getsockname(socket, (struct sockaddr *) &addr, &len) == -1) {
        printf("getsockname failed");
        return os_strdup("0.0.0.0");
    }

    OS_assert(addr.sin_family == AF_INET);
    return os_strdup(inet_ntoa(addr.sin_addr));  /* XXX not thread safe */
}

int OSReadLineFromSocket(int fd, char *line, int max)
{
    char *start;
    int ret;

    start = line;
    while (max > 0) {
//        ret = read(fd, line, 1);
		ret = recv(fd, line, 1, 0);
        if (ret == SOCKET_ERROR) {

            if (errno == EAGAIN) continue;
            if (errno == EINTR) continue;
            printf("read failed");
            return -1;
        }
        if (ret == 0)
            break;
        ++line;
        --max;
        if (line[-1] == '\n')
            break;
    }

    if (line == start)
        return 0;

    if (line[-1] == '\n')
        --line;
    if (line[-1] == '\r')
        --line;
    *line = '\0';

    return line - start;
}


int OSReadSocketToEof(int fd, char **data, size_t *len)
{
    size_t size;
    int ret;
    char *p;

    *len = 0;
    size = 0;
    *data = NULL;
    for (;;) {
        if (*len == size) {
            size += 16 * 1024;
//            p = (char *)os_realloc(*data, size);
            p = (char *)realloc(*data, size);
            *data = p;
        }
//        ret = read(fd, *data + *len, size - *len);
        ret = recv(fd, *data + *len, size - *len, 0);
        if (ret == SOCKET_ERROR) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN) continue;
            printf("Error while reading");
            goto error;
        }
        if (ret == 0)
            break;
        *len += ret;
    }

    return 0;

error:
    free(*data);
    *data = NULL;
    return -1;
}

/**
 * Check if there is something to be read in 'fd'. Return 1 if there
 * is data, 0 otherwise, -1 on error 
 */
int OSReadAvailable(int fd, long wait_usec)
{
    fd_set rf;
    struct timeval to;
    int ret;
    div_t waits;

    OS_assert(fd >= 0);

    FD_ZERO(&rf);
    FD_SET((unsigned)fd, &rf);
    waits = div(wait_usec, 1000000);
    to.tv_sec = waits.quot;
    to.tv_usec = waits.rem;
retry:
    ret = select(FD_SETSIZE, &rf, NULL, NULL, &to);

	// only on result set, so we need not use FD_ISSET macro, Zhang Yuan, 2001-7-30
    if (ret > 0 /*&& FD_ISSET(fd, &rf)*/)
        return 1;

    if (ret < 0) {
        /* In most select() implementations, to will now contain the
         * remaining time rather than the original time.  That is exactly
         * what we want when retrying after an interrupt. */
        switch (errno) {
            /*The first two entries here are OK*/
        case EINTR:
            goto retry;
        case EAGAIN:
            return 1;
            /* We are now sucking mud, figure things out here
             * as much as possible before it gets lost under
             * layers of abstraction.  */
        case EBADF:
            if (!FD_ISSET(fd, &rf)) {
                warning(0, "Tried to select on fd %d, not in the set!\n", fd);
            } else {
                warning(0, "Tried to select on invalid fd %d!\n", fd);
            }
            break;
        case EINVAL:
            /* Solaris catchall "It didn't work" error, lets apply
             * some tests and see if we can catch it. */

            /* First up, try invalid timeout*/
            if (to.tv_sec > 10000000)
                warning(0, "Wait more than three years for a select?\n");
            if (to.tv_usec > 1000000)
                warning(0, "There are only 1000000 usec in a second...\n");
            break;
	#ifdef WIN32
		case WSAENOTSOCK:
			/*
			 * Under windows platfor, it means this socket had been closed!.
			 */
			error(GetLastError(), "Select error, %d.", GetLastError());
			return -2;
	#endif
        }
        return -1; 	/* some error */
    }
    return 0;
}

// Set multicast address and working port
int OSSetMuliticastAddress(char *sMulticastAddress, long nPort)
{
	char *p = sMulticastAddress;

	if (strlen(sMulticastAddress) <= 0 || strlen(sMulticastAddress) > 15) // 15 means ip address length
		return -1;

	RETVIF(sMulticastAddress, -1);

	for( ; p && *p ; p++)
	{
		if (!isdigit(*p) && *p != '.')
		{
			printf("Error multicast address:%s\n", sMulticastAddress);
			return -1;
		}
	}

	// copy address
	strcpy(g_sMulticastAddress, sMulticastAddress);
	g_nMultiCastPort = nPort;

	return 0;
}

// Add one interface to a multicast group
int OSAddToMultiGroup(int *fd, char *if_ip)
{
	int i = 0;
	struct ip_mreq mreq;
	unsigned long interface_address;
	char		  interface_name[16];

	RETVIF(if_ip, -1);

	if (!IsIP(if_ip))
		return -1;
	/*
	 * We can change the outgoing interface.
	 * Each multicast transmission is sent from a single network interface,
	 * even if the host has more than one multicast-capable interface.
	 */
	if (strlen(if_ip) != 0 && strcmp(if_ip, "*") != 0)
	{
		//unsigned long interface_address = m_lServerAddress;
		interface_address = inet_addr(if_ip);
		strcpy(interface_name, if_ip);
		
		if (setsockopt(*fd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&interface_address, sizeof(interface_address)) < 0)
		{
		#ifdef WIN32
			printf("setsockopt(IP_MULTICAST_IF), rc=%d\n", WSAGetLastError());
		#else
			printf("setsockopt(IP_MULTICAST_IF), rc=%d\n", errno);
		#endif
			return -1;
		}
		printf("set multicast interface=%s on socket %d\n", interface_name,	*fd);
	}
	else
	{
		interface_address = INADDR_ANY;
		strcpy(interface_name, "INADDR_ANY");
	}
	/*
     * Join the multicast group
	 */
	printf("Multicast address:%s, interface:%s\n", g_sMulticastAddress,	interface_name);
	mreq.imr_multiaddr.s_addr = inet_addr(g_sMulticastAddress);
	mreq.imr_interface.s_addr = interface_address;
	
	if (setsockopt(*fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char *)&mreq,sizeof(mreq)) < 0)
	{
	#ifdef WIN32
		printf("setsockopt(IP_ADD_MEMBERSHIP), rc=%d\n",WSAGetLastError());
	#else
		printf("setsockopt(IP_ADD_MEMBERSHIP), rc=%d\n", errno);
	#endif
		return -1;
	}
	printf("set multicast membership on socket %d\n", *fd);

return 0;
}

// Set multicast TTL, socket fd and ttl value. It returns -1 if some errors occur.
int OSSetUDPMulticastTTL(int *fd, int ttl)
{
	RETVIF(fd, -1);

	if (*fd <= 0)
	{
		*fd = OSCreateUDPSocket();
	}

	if (*fd <= 0)
		return -1;

	if (setsockopt(*fd, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(int)) < 0)
	{
	#ifdef WIN32
		printf("setsockopt(IP_MULTICAST_TTL), rc=%d\n",WSAGetLastError());
	#else
		printf("setsockopt(IP_MULTICAST_TTL), rc=%d\n", errno);
	#endif
		return -1;
	}

	return 0;
}

int OSCreateUDPSocket(void)
{
    int s;

    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        printf("Couldn't create a UDP socket");
        return -1;
    }

    return s;
}

// bind a port and an interface with a socket, you can use '*' as unkown interface
int OSUDPBind(int *fd, int port, const char *interface_name)
{
	int s;
    struct sockaddr_in sa;
    struct hostent hostinfo;

	RETVIF(fd, -1);

	s = *fd;
	if (s <= 0)
	{
		s = socket(PF_INET, SOCK_DGRAM, 0);
		if (s == -1) 
		{
	        fprintf(stdout, "Couldn't create a UDP socket\n");
			return -1;
		}

		*fd = s;
	}

    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    if (interface_name == NULL || strcmp(interface_name, "*") == 0)
        sa.sin_addr.s_addr = htonl(INADDR_ANY);
    else {
        if (OSGetHostByName(&hostinfo, interface_name) == -1) {
            fprintf(stdout, "gethostbyname failed\n");
            return -1;
        }
        sa.sin_addr = *(struct in_addr *) hostinfo.h_addr;
    }

    if (bind(s, (struct sockaddr *) &sa, (int) sizeof(sa)) == -1) 
	{
#ifndef WIN32
        fprintf(stdout, "Couldn't bind a UDP socket to the [%s:%d] with error:%s\n", interface_name, port, strerror(errno));
#else
		fprintf(stdout, "Couldn't bind a UDP socket to the [%s:%d] with error:%d\n", interface_name, port, WSAGetLastError());
#endif
		CloseSocket(&s);
        return -1;
    }

    return s;
}


#if TODO
Octstr *udp_create_address(Octstr *host_or_ip, int port)
{
    struct sockaddr_in sa;
    struct hostent h;

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    if (strcmp(Octstr_get_cstr(host_or_ip), "*") == 0) {
        sa.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (OS_gethostbyname(&h, Octstr_get_cstr(host_or_ip)) == -1) {
            error(0, "Couldn't find the IP number of `%s'",
                  Octstr_get_cstr(host_or_ip));
            return NULL;
        }
        sa.sin_addr = *(struct in_addr *) h.h_addr_list[0];
    }

#ifndef WIN32
    return Octstr_create_from_data((char *) &sa, sizeof(sa));
#else
    return (Octstr *)Octstr_create_from_data((char *) &sa, sizeof(sa));
#endif
}
#endif


//int udp_get_port(Octstr *addr)
//{
//    struct sockaddr_in sa;
//
//    os_assert(Octstr_len(addr) == sizeof(sa));
//    memcpy(&sa, Octstr_get_cstr(addr), sizeof(sa));
//    return ntohs(sa.sin_port);
//}


//Octstr *udp_get_ip(Octstr *addr)
//{
//    struct sockaddr_in sa;
//
//    os_assert(Octstr_len(addr) == sizeof(sa));
//    memcpy(&sa, Octstr_get_cstr(addr), sizeof(sa));
//
//#ifndef WIN32
//    return Octstr_create(inet_ntoa(sa.sin_addr));
//#else
//    return (Octstr *)Octstr_create(inet_ntoa(sa.sin_addr));
//#endif
//}

/**
 * Send a UDP message to a given address or multicast address.
 * Return values:
 *			-1 : error
 *			 0 : success
 *			+0 : bytes sent
 */
int OSUDPSendTo(int s, char *buffer, int len)
{
    struct sockaddr_in sa;
	long ret = -1;
 
    sa.sin_family = AF_INET;
    sa.sin_port	= htons(g_nMultiCastPort);
    sa.sin_addr.s_addr = inet_addr(g_sMulticastAddress);

    if (ret = sendto(s, buffer, len, 0, (struct sockaddr *) &sa, (int) sizeof(sa)) == -1) 
	{
#ifdef WIN32
		printf("Couldn't send UDP packet, error code:%d.", WSAGetLastError());
#else
        printf("Couldn't send UDP packet, error code:%d\n, errno");
#endif
        return -1;
    }

    return ret;
}

/**
 * Send a UDP message to a given address or multicast address.
 * Return values:
 *			-1 : error
 *			 0 : success
 *			+0 : bytes sent
 */
int OSUDPSendTo(int s, char *buffer, int len, char dest_ip[16], int iPort)
{
    struct sockaddr_in sa;
	long ret = -1;

    sa.sin_family = AF_INET;
    sa.sin_port	= htons(iPort);
    sa.sin_addr.s_addr = inet_addr(dest_ip);

    if (ret = sendto(s, buffer, len, 0, (struct sockaddr *) &sa, (int) sizeof(sa)) == -1) 
	{
#ifdef WIN32
		printf("Couldn't send UDP packet, error code:%d.", WSAGetLastError());
#else
        printf("Couldn't send UDP packet, error code:%d\n, errno");
#endif
        return -1;
    }

    return ret;
}

/**
 * Receive a UDP message from a client.
 * nTimeOutMilliSeconds in millionseconds
 * Return values:
 *			-1 : error
 *			 0 : no data available
 *			+0 : bytes got
 */
int OSUDPRecvFrom(int s, char *&pBuffer, long &nBufLen, char src_ip[16], long &nPort, long nTimeOutMilliSeconds)
{
    char *buf;
    int bytes;
	struct sockaddr_in addr;
	long ret = -1;
#ifdef WIN32
    int salen;
#else
	socklen_t salen;
#endif

	if (nTimeOutMilliSeconds > 0)
		if ((ret = OSReadAvailable(s, nTimeOutMilliSeconds * 1000)) < 1)
			return ret;

	buf = (char *)malloc(UDP_PACKET_MAX_SIZE);	// Added by Zhang Yuan for some leaks.

    salen = sizeof(struct sockaddr_in);
    bytes = recvfrom(s, buf, UDP_PACKET_MAX_SIZE, 0,
                     (struct sockaddr *)&addr, &salen);
    if (bytes == -1) 
	{
        if (errno != EAGAIN)
#ifdef WIN32
		printf("Couldn't receive UDP packet, error code:%d.\n", WSAGetLastError());
#else
        printf("Couldn't receive UDP packet, error :%s.\n", strerror(errno));
#endif
		else
			printf("Please retry recvfrom() call!\n");

		SAFEFREE(buf);	// safe
		return -1;
    }

	// allocate space
	pBuffer = new char [ bytes ];
	if (!pBuffer)
	{
        printf("Couldn't allocate enough memory space, error :%s.\n", strerror(errno));
		SAFEFREE(buf);
		return -1;
	}

	// copy data
	nBufLen = bytes;
	memcpy(pBuffer, buf, bytes);

	// release unused memory space
	SAFEFREE(buf);

	if (src_ip)
	{
#ifdef WIN32
		_snprintf(src_ip, 16, "%u.%u.%u.%u", 
			((unsigned char *)&addr.sin_addr.s_addr)[0], ((unsigned char *)&addr.sin_addr.s_addr)[1], 
			((unsigned char *)&addr.sin_addr.s_addr)[2], ((unsigned char *)&addr.sin_addr.s_addr)[3]);
#else
		snprintf(src_ip, 16, "%u.%u.%u.%u", 
			((unsigned char *)&addr.sin_addr.s_addr)[0], ((unsigned char *)&addr.sin_addr.s_addr)[1], 
			((unsigned char *)&addr.sin_addr.s_addr)[2], ((unsigned char *)&addr.sin_addr.s_addr)[3]);
#endif
	}

	nPort = addr.sin_port;

    return bytes;
}




//Octstr *get_official_name(void)
//{
//    os_assert(official_name != NULL);
//    return official_name;
//}
const long OSGetLocalIPLong(void)
{
//	os_assert(official_name != NULL);
	return inet_addr(g_strHostName);
}

const char *OSGetLocalIPString(void)
{
	return g_strHostIP;
}

static void setup_official_name(void)
{
	long nIP = 0;

#ifndef WIN32
    struct utsname u;
	struct hostent h;

//    os_assert(official_name == NULL);
    if (uname(&u) == -1)
        panic(0, "uname failed - can't happen, unless this machine is buggy.\n");
    if (OS_gethostbyname(&h, u.nodename) == -1) 
	{
        error(0, "Can't find out official hostname for this host, "
              "using `%s' instead.", u.nodename);
//        official_name = Octstr_create(u.nodename);
		strcpy(g_strHostName, u.nodename);
    } else 
	{
		strcpy(g_strHostName, h.h_name);
//		strcpy(g_strHostIP,  h.h_name);
//        official_name = Octstr_create(h.h_name);
    }
#else
	// added by chenglei 2001/1/16
	int GetHostAddrInRegister(char* strHostIP);
	// end
//	os_assert(official_name == NULL);
	

	if (gethostname(g_strHostName, sizeof(g_strHostName)) == 0)
	{
		if (OSGetHostByName(&localhostinfo, g_strHostName) == -1)
		{
			error(0, "Can't find out official hostname for this host, "
				"using '%s' instead.", g_strHostName);
//				official_name = (Octstr *)Octstr_create_real(g_strHostName);
		}else
		{
//				official_name = (Octstr *)Octstr_create_real(localhostinfo.h_name);
		}
	}else
	{
		error(0, "Error code:%d.\n", WSAGetLastError());
		panic(0, "gethostname failed - can't happen, unless this machine is buggy.\n");
	}

//	else
//	{
//		char** pAddrList = (char**)malloc(sizeof(char*));
//		unsigned long* ipAddr = (unsigned long*)malloc(sizeof(unsigned long));
//		if (ipAddr != NULL && pAddrList != NULL)
		{
//			official_name = (Octstr *)Octstr_create_real(g_strHostName);
//			sscanf(g_strHostName, "%u.%u.%u.%u", &(((unsigned char*)ipAddr)[0]),
//				&(((unsigned char*)ipAddr)[1]),
//				&(((unsigned char*)ipAddr)[2]),
//				&(((unsigned char*)ipAddr)[3]));
//			localhostinfo.h_addr_list = pAddrList;
//			localhostinfo.h_addr_list[0] = (char*)ipAddr;
		}
//		else
//			panic(0, "malloc fail at File: %s, Line:%s.", __FILE__, __LINE__);
//	}
#endif
	
	nIP = inet_addr(g_strHostName);

	snprintf(g_strHostIP, 20, "%u.%u.%u.%u", ((unsigned char *)&nIP)[0], ((unsigned char *)&nIP)[1], 
		((unsigned char *)&nIP)[2], ((unsigned char *)&nIP)[3]);
}

/*
 * Close the socket under different OS.
 */
void CloseSocket(int *fd)
{
#ifdef WIN32
	closesocket(*fd);
#else
	close(*fd);
#endif
	*fd = -1;
	return ;
}

// mainly for Windows platform
void OSSocketInit(void)
{

	if (iInitilizated++)
		return;

#ifdef WIN32
	WSADATA WSAData;
	/*
	Load socket library
	*/
	if (WSAStartup(MAKEWORD(2,2),&WSAData) != 0)
	{
		printf("WSAStartup, rc=%d\n",WSAGetLastError());
		exit(1);
	}
#endif
//    inet_mutex = mutex_create();
//    setup_official_name();
}

void OSSocketShutdown(void)
{
//    mutex_destroy(inet_mutex);
//    Octstr_destroy(official_name);
//    official_name = NULL;
	static int iShutdowned = 0;

	if (--iInitilizated)
		return;

#ifdef WIN32
	WSACleanup();
#endif
}
