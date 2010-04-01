
#ifndef _OS_SOCKET_H
#define _OS_SOCKET_H

#include "osmacros.h"

#ifndef UDP_PACKET_MAX_SIZE
#define UDP_PACKET_MAX_SIZE (64*1024)
#endif

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
 typedef int socklen_t;
#endif


/* Return the official and fully qualified domain name of the host. 
   Caller should treat this as read-only. Caller MUST NOT destroy it. */
//Octstr *get_official_name(void);

/**
 * <pre>
 * Get the string of first ip address of localhost.
 * Parameters: 
 *				None
 *
 * Return value:
 *				const long, the ip address
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD const char *OSGetLocalIPString(void);

/**
 * <pre>
 * Get the inet of the first ip address of localhost.
 * Parameters: 
 *				None
 *
 * Return value:
 *				const long, the inet address
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD const long OSGetLocalIPLong(void);

/**
 * <pre>
 * Open a server socket, meaning listening one a give port.
 * Parameters: 
 *				port, tcp listening port
 * Return value:
 *				-1 for error, >= 0 socket number
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSMakeServerTCPSocket(int port);

/**
 * <pre>
 * Open a client socket.
 * Parameters: 
 *				hostname, string name of remote(server) box
 *				port, tcp port on destination box
 * Return value:
 *				-1 for error, >= 0 socket number
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSTCPConnectWithName(char *hostname, int port);

/**
 * <pre>
 * As above, but binds our port to 'our_port'.
 * Parameters: 
 *				hostname, string name of remote(server) box
 *				port, tcp port on destination box
 *				our_port, local tcp port
 * Return value:
 *				-1 for error, >= 0 socket number
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSTCPConnectWithNamePort(char *hostname, int port, int our_port);

/**
 * <pre>
 * As above, but using ip instead of using hostname, Zhang Yuan, 2001-1-3.
 * Parameters: 
 *				ip, ip address of remote(server) box
 *				port, tcp port on destination box
 * Return value:
 *				-1 for error, >= 0 socket number
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSTCPConnectWithIP(char *ip, int port);

/**
 * <pre>
 * As above, but using ip instead of using hostname, Zhang Yuan, 2001-1-3.
 * Parameters: 
 *				ip, ip address of remote(server) box
 *				port, tcp port on destination box
 *				our_port, local tcp port
 * Return value:
 *				-1 for error, >= 0 socket number
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSTCPConnectWithIPPort(char *ip, int port, int our_port);

/**
 * <pre>
 * Write string to socket. 
 * Parameters: 
 *				socket, tcp connection socket
 *				str, content string
 * Return values:
 *			-1 : error
 *			 0 : success
 *			+0 : bytes written
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSWriteToSocket(int socket, char *str);

/**
 * <pre>
 * Write a binary buffer to socket.
 * Added by Gong Zhangwei, modified by Zhang Yuan
 * Parameters: 
 *				socket, tcp connection socket
 *				buf, content buffer
 *				len, buffer length
 * Return values:
 *			-1 : error
 *			 0 : success
 *			+0 : bytes written
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSTCPSend(int socket, char *buf, int len);

/**
 * <pre>
 * Read a binary buffer from socket.
 * Added by Gong Zhangwei, modified by Zhang Yuan
 * Parameters: 
 *				socket, tcp connection socket
 *				buf, content buffer
 *				max, maximum buffer length
 *				fill_up, false or true. Whether to fill this buffer
 *				nTimeOutMilliSeconds, timeout value in millionseconds with default is 0
 * Return values:
 *			-1 : error
 *			 0 : success
 *			+0 : bytes written
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSTCPRecv(int socket, char *buf, int max, bool fill_up = false, long nTimeOutMilliSeconds = 0);

/**
 * <pre>
 * Check if socket is in blocking or non-blocking mode.  Return -1 for
 * Parameters: 
 *				socket, tcp connection socket
 * Return values:
 *			-1 : error
 *			 0 : nonblocking
 *			+0 : blocking
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSIsSocketBlock(int socket);

/**
 * <pre>
 * Set socket to blocking or non-blocking mode.
 * Parameters: 
 *				socket, tcp connection socket
 *				blocking, block flag, 1 for block, 0 for unblock
 * Return values:
 *			-1 : error
 *			 0 : nonblocking
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSSetBlockSocket(int socket, int blocking);

/**
 * <pre>
 * Get IP address from one given socket, and you should delete this char* results
 * Parameters: 
 *				socket, tcp connection socket
 * Return values:
 *				NULL - failure
 *				!NULL- the IP address of the peer of a connected socket. Return value
 * is a dynamically allocated C string, which the caller is expected to free it.
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD char *OSSocketGetPeerIP(int socket);

/**
 * <pre>
 * Read a line from a socket. Return -1 for error, 0 for EOF, or 1 for OK.
   Remove CRLF and LF from end of line. 
 * Parameters: 
 *				fd, tcp connection socket
 *				line, buffer pointer
 *				max, maximum buffer length
 * Return values:
 *			-1 : error
 *			 0 : success but zero length
 *			+0 : bytes read
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSReadLineFromSocket(int fd, char *line, int max);

/**
 * <pre>
 * Read the rest of the incoming socket into dynamically allocated memory. 
 * Parameters: 
 *				fd, tcp connection socket
 *				data, buffer pointer
 *				len, maximum buffer length
 * Return values:
 *			-1 : error
 *			 0 : success
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSReadSocketToEof(int fd, char **data, size_t *len);

/**
 * <pre>
 * Check if there is something to be read in 'fd'.
 * Parameters: 
 *				fd, tcp connection socket
 *				wait_usec, timeout 
 * Return values:
 *			-1 : error
 *			 0 : success
 *			 1 : data is available
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSReadAvailable(int fd, long wait_usec);

/**
 * <pre>
 * Create a UDP socket for receiving from clients. Return -1 for failure,
 * a socket file descriptor >= 0 for OK.
 * Parameters: 
 *				fd, tcp connection socket
 *				port, udp port to be binded
 *				interface_name, interface ip address
 * Return values:
 *			 <=0 : error
 *			  >0 : scoket number
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSUDPBind(int *fd, int port, const char *interface_name = "0.0.0.0");

/**
 * <pre>
 * Create the client end of a UDP socket (i.e., a UDP socket that can
 * be on any port). 
 * Parameters: 
 *				None
 * Return values:
 *			 <=0 : error
 *			  >0 : scoket number
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSCreateUDPSocket(void);

// Multicast functions
// Multicast mode
// Sender					Receiver
// ----------------------------------
//  socket()				socket()
//   V						  V
//  bind()			add into multicast group
//   V						  V
//sento(multicast address)	bind()
//   V						  V
//   V					  recvfrom()
//   V            			  V
//   V					      V
//   V					got and process
//   V					      V
// ......					.....

// Sender called functions and order
//  OSCreateUDPSocket() --> OSSetUDPMulticastTTL() --> OSUDPSendTo()

// Receiver called functions and order
//  OSCreateUDPSocket() --> OSUDPBind() --> OSSetUDPMulticastTTL() --> OSAddIfToMulticastGroup() --> OSUDPRecvFrom()

/**
 * <pre>
 * Set multicast TTL, socket fd and ttl value.
 * Parameters: 
 *				fd, tcp connection socket
 *				ttl, Time To Live
 * Return values:
 *			  <0 : error
 *			   0 : success
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSSetUDPMulticastTTL(int *fd, int ttl);

/**
 * <pre>
 * Set multicast address.
 * Parameters: 
 *				sMulticastAddress, multicast ip address
 *				nPort, udp port, default is 2380
 * Return values:
 *			  <0 : error
 *			   0 : success
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSSetMuliticastAddress(char *sMulticastAddress, long nPort = 2380);

/**
 * <pre>
 * Add one interface to a multicast group.
 * Parameters: 
 *				fd, socket number to be added into a multicast group
 *				if_ip, interface ip address, default is '*'
 * Return values:
 *			  <0 : error
 *			   0 : success
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSAddToMultiGroup(int *fd, char *if_ip);

/**
 * Encode a hostname or IP number and port number into a binary address,
 * and return that as an Octstr. Return NULL if the host doesn't exist
 * or the IP number is syntactically invalid, or the port is bad.
 */
//Octstr *udp_create_address(Octstr *host_or_ip, int port);


/**
 * Return the IP number of an encoded binary address, as a cleartext string.
 */
//Octstr *udp_get_ip(Octstr *addr);


/*
 * Return the port number of an encoded binary address, as a cleartext string.
 */
//int udp_get_port(Octstr *addr);

/**
 * <pre>
 * Send a UDP message to a multicast address.
 * Parameters: 
 *				s, udp socket number
 *				buffer, content buffer
 *				len, content length
 * Return values:
 *			-1 : error
 *			 0 : success
 *			+0 : bytes sent
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSUDPSendTo(int s, char *buffer, int len);

/**
 * <pre>
 * Send a UDP message to a given address.
 * Parameters: 
 *				s, udp socket number
 *				buffer, content buffer
 *				len, content length
 *				dest_ip, ip address of destination box
 *				iPort, destination port
 * Return values:
 *			-1 : error
 *			 0 : success
 *			+0 : bytes sent
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSUDPSendTo(int s, char *buffer, int len, char dest_ip[16], int iPort);


/**
 * <pre>
 * Receive a UDP message from a client.
 * Parameters: 
 *				s, udp socket number
 *				pBuffer, buffer pointer which contains all data received
 *				nBufLen, length of received data
 *				src_ip,  source ip address
 *				nPort,	 udp port
 *				nTimeOutMilliSeconds, timeout value in millionseconds
 * Return values:
 *			-1 : error
 *			 0 : no data available
 *			+0 : bytes got
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSUDPRecvFrom(int s, char *&pBuffer, long &nBufLen, char src_ip[16], long &nPort, long nTimeOutMilliSeconds = 0);

/**
 * <pre>
 * Close one socket under different OS.
 * Parameters: 
 *				fd, memory address of one socket variable
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void CloseSocket(int *fd);

/**
 * <pre>
 * This must be called before sockets are used. gwlib_init does that
 * Parameters: 
 *				None
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSSocketInit(void);


/**
 * <pre>
 * Likewise, shutdown, called by gwlib_shutdown
 * Parameters: 
 *				None
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSSocketShutdown(void);

#endif
