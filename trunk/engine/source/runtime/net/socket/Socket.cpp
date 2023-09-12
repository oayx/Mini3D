#include "Socket.h"
#if defined(DC_PLATFORM_WIN32)
#include <ws2tcpip.h>
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#define INVALID_SOCKET  (int)(~0)
#define SOCKET_ERROR    (-1)
#define WSAGetLastError() errno
#define WSAEWOULDBLOCK  EAGAIN
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Socket, Object);
Socket::Socket()
{
#if defined(DC_PLATFORM_WIN32)
	WSAData wsad;
	int result = ::WSAStartup(MAKEWORD(2,2), &wsad);
	if (result != 0)Debuger::Error("WSAStartup error:%d", result);
#endif
	m_nSocket = INVALID_SOCKET;
}
Socket::~Socket()
{
	Close();
}
int Socket::Bind(const struct sockaddr_in *addr)
{
	struct sockaddr_in addrin;
	memset(&addrin, 0, sizeof(addrin));
	addrin.sin_family = AF_INET;
	addrin.sin_addr.s_addr = addr->sin_addr.s_addr;
	addrin.sin_port = addr->sin_port;

	int err = ::bind( m_nSocket, (sockaddr*)&addrin, sizeof(addrin) );
	if ( err != 0 )
	{
		Debuger::Error("bind socket error:%d", WSAGetLastError());
	}

	return err ? WSAGetLastError() : 0;
}
int Socket::Bind(const char* host_name, int port)
{
	struct sockaddr_in addrIn;
	int err = ResolveHost(host_name, &addrIn.sin_addr);
	if (err) return err;
	addrIn.sin_port = ::htons(port);

	return Bind(&addrIn);
}
int Socket::Listen(int backlog)
{
	int err = ::listen( m_nSocket, backlog );
	if (err != 0)
	{
		Debuger::Error("listen socket error:%d", WSAGetLastError());
		return WSAGetLastError();
	}

	return 0;
}
int Socket::Connect(const addrinfo *addr)
{
	int err = -1;
	const addrinfo *pAddr = NULL;
	for (pAddr = addr; pAddr; pAddr = pAddr->ai_next)
	{
		m_nSocket = socket(pAddr->ai_family, pAddr->ai_socktype, pAddr->ai_protocol);
		if (m_nSocket < 0)
			continue;
		err = ::connect(m_nSocket, addr->ai_addr, (int)addr->ai_addrlen);
		if (err == 0)
			break;
	}
	if (err == 0)
	{
		_isClose = false;
		DoConnected();
	}
	else
	{
		m_nSocket = INVALID_SOCKET;
		err = WSAGetLastError();
		if (err == WSAEWOULDBLOCK)
		{
			err = 0;
		}
		else
		{
			Debuger::Error("connect socket error:%d", WSAGetLastError());
		}
	}

	return err;
}
int Socket::Connect(const struct sockaddr_in *addr)
{
	struct sockaddr_in addrin;
	memset(&addrin, 0, sizeof(addrin));
	addrin.sin_family = AF_INET;
	addrin.sin_addr.s_addr = addr->sin_addr.s_addr;
	addrin.sin_port = addr->sin_port;

	int err = ::connect(m_nSocket, (sockaddr*)&addrin, sizeof(addrin));
	if ( err == 0 )
	{
		_isClose = false;
		DoConnected();
	}
	else 
	{
		err = WSAGetLastError();
		if ( err == WSAEWOULDBLOCK )
		{
			err = 0;
		}
		else
		{
			Debuger::Error("connect socket error:%d", WSAGetLastError());
		}
	}

	return err;
}
int Socket::Connect(const char* host_name, int port)
{
	struct addrinfo *result = NULL;
	int err = ResolveHostV6(host_name, &result);
	if (err || result == NULL)
		return err;
	sockaddr_in* pSockaddr_in = (sockaddr_in*)result->ai_addr;
	pSockaddr_in->sin_port = ::htons(port);
	return Connect(result);
}
int Socket::Accept(int64 *socket, int wait_msec, struct sockaddr_in *remoteAddr)
{
	int err;
	fd_set set;
	timeval tv;
#if defined(DC_PLATFORM_WIN32)
	int addrsize;
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	socklen_t addrsize;
#endif

	FD_ZERO( &set );
	FD_SET( m_nSocket, &set );

	tv.tv_sec = long(wait_msec / 1000);
	tv.tv_usec = long(wait_msec * 1000);

	err = ::select( int(m_nSocket + 1), &set, NULL, NULL, &tv );
	if (err < 0)
	{
		Debuger::Error("select socket error:%d", WSAGetLastError());
		return WSAGetLastError();
	}
	if ( err > 0 )
	{
		addrsize = sizeof(*remoteAddr);
		*socket = ::accept( m_nSocket, (sockaddr*)remoteAddr, &addrsize );
		if (*socket == INVALID_SOCKET)
		{
			Debuger::Error("accept socket error:%d", WSAGetLastError());
			return WSAGetLastError();
		}

		return 0;
	}

	return SOCKET_ERROR - 1;
}
int Socket::Recv(void* buf, int len, const int flags)
{
#ifndef DC_PLATFORM_WIN32
	//flags |= MSG_NOSIGNAL;
#endif
	int err = ::recv(m_nSocket, (char*)buf, len, flags);
	if (err == 0)
	{
		this->Close();
	}
	else if (err < 0)
	{
		if (_isNonBlocking)
		{
			err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK)
			{
				DoSocketError(m_nSocket, err);
				err = -1;
			}
			else err = SOCKET_ERROR - 1;
		}
	}

	return err;
}
int Socket::Recv(int64 socket, void* buf, int len, const int flags)
{
#ifndef DC_PLATFORM_WIN32
	//flags |= MSG_NOSIGNAL;
#endif
	int err = ::recv(socket, (char*)buf, len, flags);
	if (err == 0)
	{//如果客户端关闭时不调用close，则收不到这个消息
		this->ClientClose(socket);
	}
	else if (err < 0)
	{
		if (_isNonBlocking)
		{
			err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK)
			{
				DoSocketError(socket, err);
				err = -1;
			}
			else err = SOCKET_ERROR - 1;
		}
	}

	return err;
}
int Socket::Send(const void* buf, int len, const int flags)
{
	int nRet, err;
	char *ptr = (char*)buf;

#ifndef DC_PLATFORM_WIN32
	//flags |= MSG_NOSIGNAL;
#endif
	nRet = 0;
	while ( len > 0 )
	{//对于非阻塞SOCK_STREAM类型的套接口，实际写的数据数目可能在1到所需大小之间，其值取决于本地和远端主机的缓冲区大小
		err = ::send( m_nSocket, (char*)ptr, len, flags );
		if ( err == 0 )
		{
			nRet = 0;
			this->Close();
			break;
		}
		else if ( err < 0 )
		{
			if ( _isNonBlocking )
			{
				err = WSAGetLastError();
				if ( err != WSAEWOULDBLOCK )
				{
					nRet = SOCKET_ERROR;
					DoSocketError(m_nSocket, WSAGetLastError());
				}
				else if ( nRet == 0 )
				{
					nRet = SOCKET_ERROR - 1;
				}
			}
			break;
		}
		else 
		{//发送剩余
			nRet += err;
			ptr += err;
			len -= err;
		}
	}
	return nRet;
}
int Socket::RecvFrom(const char* host_name, int port, char* buf, int len, const int flags)
{
	struct sockaddr_in addrIn;
	int err = ResolveHost(host_name, &addrIn.sin_addr);
	if (err)return err;
	addrIn.sin_port = ::htons(port);
	addrIn.sin_family = SOCK_DGRAM;
	socklen_t addrLen = sizeof(addrIn);
	
	err = ::recvfrom(m_nSocket, buf, len, flags, (sockaddr*)&addrIn, &addrLen);
	if (err == SOCKET_ERROR)
	{
		if (_isNonBlocking)
		{
			err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK)
			{
				Debuger::Error("recvfrom error:%d", WSAGetLastError());
				return -1;
			}
			else err = SOCKET_ERROR - 1;
		}
	}
	return err;
}
int Socket::SendTo(const char* host_name, int port, const char* buf, int len, const int flags)
{
	struct sockaddr_in addrIn;
	int err = ResolveHost(host_name, &addrIn.sin_addr);
	if (err)return err;
	addrIn.sin_port = ::htons(port);
	addrIn.sin_family = SOCK_DGRAM;

	int nRet = 0;
	char *ptr = (char*)buf;
	while (len > 0)
	{//如果传送系统的缓冲区空间不够保存需传送的数据，除非套接口处于非阻塞I/O方式，否则sendto()将阻塞
		err = ::sendto(m_nSocket, ptr, len, flags, (sockaddr*)&addrIn, sizeof(addrIn));
		if (err == SOCKET_ERROR)
		{
			nRet = 0;
			Debuger::Error("sendto error:%d", WSAGetLastError());
			break;
		}
		else
		{//发送剩余
			nRet += err;
			ptr += err;
			len -= err;
		}
	}
	return nRet;
}
void Socket::Close()
{
	_isClose = true;
	_isNonBlocking = false;
	if (m_nSocket != INVALID_SOCKET)
	{
		CloseSocket(m_nSocket);
		DoClosed();
		m_nSocket = INVALID_SOCKET;
	}
}
void Socket::ClientClose(int64 socket)
{
	DoClientClosed(socket);
}
int Socket::SetNonBlocking(bool nonBlocking)
{
	if (nonBlocking != _isNonBlocking)
	{
		int err = SetNonBlocking(m_nSocket, nonBlocking);
		if (err == 0)
		{
			_isNonBlocking = nonBlocking;
		}
		return err;
	}
	return 0;
}
int Socket::CreateSocket(int64& socket, const int af /* = AF_INET */, const int type /* = SOCK_STREAM */, const int protocol /* = IPPROTO_TCP */)
{
	socket = ::socket(af, type, protocol);
	if (socket == INVALID_SOCKET)
	{
		Debuger::Error("create socket error:%d", WSAGetLastError());
		return WSAGetLastError();
	}

	return 0;
}
int Socket::CloseSocket(int64 socket)
{
	if (socket != INVALID_SOCKET)
	{
		int err = 0;
#if defined(DC_PLATFORM_WIN32)
		err = ::closesocket(socket);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
		err = ::close(socket);
#endif
		if(err)
		{
			Debuger::Error("CloseSocket error:%d", WSAGetLastError());
			return WSAGetLastError();
		}
	}
	return 0;
}
int Socket::GetBufferSize(int64 socket, uint &recvSize, uint &sendSize)
{
#if defined(DC_PLATFORM_WIN32)
	int oplenR = sizeof(recvSize);
	int oplenS = sizeof(sendSize);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	socklen_t oplenR = sizeof(recvSize);
	socklen_t oplenS = sizeof(sendSize);
#endif
	int err;
	err = ::getsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)&recvSize, &oplenR);
	if (err) 
	{
		Debuger::Error("GetBufferSize error:%d", WSAGetLastError());
		return WSAGetLastError();
	}

	err = ::getsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&sendSize, &oplenS);
	if (err) 
	{
		Debuger::Error("GetBufferSize error:%d", WSAGetLastError());
		return WSAGetLastError();
	}

	return 0;
}
int Socket::SetBufferSize(int64 socket, uint recvSize, uint sendSize)
{
	int err;
	err = ::setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)&recvSize, sizeof(recvSize));
	if (err) 
	{
		Debuger::Error("SetBufferSize error:%d", WSAGetLastError());
		return WSAGetLastError();
	}

	err = ::setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&sendSize, sizeof(sendSize));
	if (err) 
	{
		Debuger::Error("SetBufferSize error:%d", WSAGetLastError());
		return WSAGetLastError();
	}

	return 0;
}
int Socket::SetSendRecvTimeout(int64 socket, uint recvTime, uint sendTime)
{
	int err;
	err = ::setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTime, sizeof(recvTime));
	if (err) 
	{
		Debuger::Error("SetSendRecvTimeout error:%d", WSAGetLastError());
		return WSAGetLastError();
	}

	err = ::setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&sendTime, sizeof(sendTime));
	if (err) 
	{
		Debuger::Error("SetSendRecvTimeout error:%d", WSAGetLastError());
		return WSAGetLastError();
	}

	return 0;
}
int Socket::SetNonBlocking(int64 socket, bool nonBlocking)
{
#if defined(DC_PLATFORM_WIN32)
	u_long ulock;
	ulock = nonBlocking ? 1 : 0;
	if (::ioctlsocket(socket, FIONBIO, &ulock))
	{
		Debuger::Error("ioctlsocket socket error:%d", WSAGetLastError());
		return WSAGetLastError();
	}
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	int flags = ::fcntl(socket, F_GETFL);
	if (flags < 0)
	{
		Debuger::Error("fcntl error:%d", WSAGetLastError());
		return WSAGetLastError();
	}
	if (nonBlocking)
		flags |= O_NONBLOCK;
	else 
		flags &= ~O_NONBLOCK;
	if (::fcntl(socket, F_SETFL, flags) < 0)
	{
		Debuger::Error("fcntl error:%d", WSAGetLastError());
		return WSAGetLastError();
	}
#endif
	return 0;
}
int Socket::SetNoDelay(int64 socket, bool nodelay)
{
	int so_nodelay = nodelay ? 1 : 0;
	int err = ::setsockopt(socket, SOL_SOCKET, TCP_NODELAY, (char*)&so_nodelay, sizeof(so_nodelay));
	if (err)
	{
		Debuger::Error("SetSendRecvTimeout error:%d", WSAGetLastError());
		return WSAGetLastError();
	}
	return 0;
}
/*
l_onoff	l_linger	closesocket行为		发送队列			底层行为
零		忽略		立即返回			保持直至发送完成	系统接管套接字并保证将数据发送至对端
非零	零			立即返回			立即放弃			直接发送RST包，自身立即复位，不用经过2MSL状态。对端收到复位错误号
非零	非零		阻塞直到l_linger时间超时或数据发送完成。(套接字必须设置为阻塞zhuan)	在超时时间段内保持尝试发送，若超时则立即放弃。超时则同第二种情况，若发送完成则皆大欢喜
*/
int Socket::SetLinger(int64 socket, bool enable, ushort time)
{
	struct linger s;
	s.l_onoff = enable ? 1 : 0;
	s.l_linger = time;

	int err = ::setsockopt(socket, SOL_SOCKET, SO_LINGER, (char*)&s, sizeof(s));
	if (err)
	{
		Debuger::Error("SetSendRecvTimeout error:%d", WSAGetLastError());
		return WSAGetLastError();
	}
	return 0;
}
int Socket::SetBroadcast(int64 socket)
{
	int so_broadcast = 1;
	int err = ::setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (char*)&so_broadcast, sizeof(so_broadcast));
	if (err)
	{
		Debuger::Error("SetSendRecvTimeout error:%d", WSAGetLastError());
		return WSAGetLastError();
	}
	return 0;
}
int Socket::SetReuseAddr(int64 socket, bool enable)
{
	int on = enable ? 1 : 0;
	int err = ::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
	if (err)
	{
		Debuger::Error("SetReuseAddr error:%d", WSAGetLastError());
		return WSAGetLastError();
	}
	return 0;
}
int Socket::ResolveHost(const char* name, struct in_addr *addr)
{
	hostent *phost;
	phost = ::gethostbyname(name);
	if ( phost )
	{
		addr->s_addr = *(u_long*)phost->h_addr_list[0];
		return 0;
	}
	else 
	{
		addr->s_addr = 0;
		Debuger::Error("ResolveHost error:%d", WSAGetLastError());
		return WSAGetLastError();
	}
}
int Socket::ResolveHostV6(const char* name, addrinfo** result)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM;
	int ret = ::getaddrinfo(name, NULL, &hints, result);
	if (0 == ret)
	{
		return ret;
	}
	else
	{
		*result = NULL;
		Debuger::Error("ResolveHostV6 error:%d", WSAGetLastError());
		return WSAGetLastError();
	}
}
DC_END_NAMESPACE
/*
WSANOTINITIALISED：	在使用此API之前应首先成功地调用WSAStartup()。
WSAENETDOWN：		WINDOWS套接口实现检测到网络子系统失效。
WSAEFAULT：			fromlen参数非法；from缓冲区大小无法装入端地址。
WSAEINTR：			阻塞进程被WSACancelBlockingCall()取消。
WSAEINPROGRESS：	一个阻塞的WINDOWS套接口调用正在运行中。
WSAEINVAL：			套接口未用bind()进行捆绑。
WSAENOTCONN：		套接口未连接（仅适用于SOCK_STREAM类型）。
WSAENOTSOCK：		描述字不是一个套接口。
WSAEOPNOTSUPP：		指定了MSG_OOB，但套接口不是SOCK_STREAM类型的。
WSAESHUTDOWN：		套接口已被关闭。当一个套接口以0或2的how参数调用shutdown()关闭后，无法再用recv()接收数据。
WSAEWOULDBLOCK：	套接口标识为非阻塞模式，但接收操作会产生阻塞。
WSAEMSGSIZE：		数据报太大无法全部装入缓冲区，故被剪切。
WSAECONNABORTED：	由于超时或其他原因，虚电路失效。
WSAECONNRESET：		远端强制中止了虚电路。
*/

/*
setsockopt()支持下列选项。其中“类型”表明optval所指数据的类型。
选项			类型	意义
SO_BINDTODEV	char *	将套接字绑定到指定端口。
SO_BROADCAST	BOOL	允许套接口传送广播信息。
SO_DEBUG		BOOL	记录调试信息。
SO_DONTLINER	BOOL	不要因为数据未发送就阻塞关闭操作。设置本选项相当于将SO_LINGER的l_onoff元素置为零。
SO_DONTROUTE	BOOL	禁止选径；直接传送。
SO_KEEPALIVE	BOOL	发送“保持活动”包。
SO_LINGER		struct linger FAR* 如关闭时有未发送数据，则逗留。
SO_OOBINLINE	BOOL	在常规数据流中接收带外数据。
SO_RCVBUF		int		为接收确定缓冲区大小。
SO_REUSEADDR	BOOL	允许套接口和一个已在使用中的地址捆绑（参见bind()）。
SO_SNDBUF		int		指定发送缓冲区大小。
TCP_NODELAY		BOOL	禁止发送合并的Nagle算法。
setsockopt()不支持的BSD选项有：
选项名			类型	意义
SO_ACCEPTCONN	BOOL	套接口在监听。
SO_ERROR		int		获取错误状态并清除。
SO_RCVLOWAT		int		接收低级水印。
SO_RCVTIMEO		int		接收超时。
SO_SNDLOWAT		int		发送低级水印。
SO_SNDTIMEO		int		发送超时。
SO_TYPE			int		套接口类型。
IP_OPTIONS		在IP头中设置选项。

应用举例
1.设置调用close(socket)后,仍可继续重用该socket。调用close(socket)一般不会立即关闭socket，而经历TIME_WAIT的过程。
	BOOL bReuseaddr = TRUE;
	setsockopt( s, SOL_SOCKET, SO_REUSEADDR, ( const char* )&bReuseaddr, sizeof( BOOL ) );
2. 如果要已经处于连接状态的soket在调用closesocket()后强制关闭，不经历TIME_WAIT的过程：
	BOOL bDontLinger = FALSE;
	setsockopt( s, SOL_SOCKET, SO_DONTLINGER, ( const char* )&bDontLinger, sizeof( BOOL ) );
3.在send(),recv()过程中有时由于网络状况等原因，收发不能预期进行,可以设置收发时限：
	int nNetTimeout = 1000; //1秒
	//发送时限
	setsockopt( socket, SOL_SOCKET, SO_SNDTIMEO, ( char * )&nNetTimeout, sizeof( int ) );
	//接收时限
	setsockopt( socket, SOL_SOCKET, SO_RCVTIMEO, ( char * )&nNetTimeout, sizeof( int ) );
4.在send()的时候，返回的是实际发送出去的字节(同步)或发送到socket缓冲区的字节(异步)；系统默认的状态发送和接收一次为8688字节(约
	为8.5K)；在实际的过程中如果发送或是接收的数据量比较大，可以设置socket缓冲区，避免send(),recv()不断的循环收发：
	// 接收缓冲区
	int nRecvBufLen = 32 * 1024; //设置为32K
	setsockopt( s, SOL_SOCKET, SO_RCVBUF, ( const char* )&nRecvBufLen, sizeof( int ) );
	//发送缓冲区
	int nSendBufLen = 32*1024; //设置为32K
	setsockopt( s, SOL_SOCKET, SO_SNDBUF, ( const char* )&nSendBufLen, sizeof( int ) );
5.在发送数据的时，不执行由系统缓冲区到socket缓冲区的拷贝，以提高程序的性能：
	int nZero = 0;
	setsockopt( socket, SOL_SOCKET, SO_SNDBUF, ( char * )&nZero, sizeof( nZero ) );
6.在接收数据时，不执行将socket缓冲区的内容拷贝到系统缓冲区：
	int nZero = 0;
	setsockopt( s, SOL_SOCKET, SO_RCVBUF, ( char * )&nZero, sizeof( int ) );
7.一般在发送UDP数据报的时候，希望该socket发送的数据具有广播特性：
	BOOL bBroadcast = TRUE;
	setsockopt( s, SOL_SOCKET, SO_BROADCAST, ( const char* )&bBroadcast, sizeof( BOOL ) );
8.在client连接服务器过程中，如果处于非阻塞模式下的socket在connect()的过程中可以设置connect()延时,直到accpet()被调用(此设置只
	有在非阻塞的过程中有显著的作用，在阻塞的函数调用中作用不大)
	BOOL bConditionalAccept = TRUE;
	setsockopt( s, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, ( const char* )&bConditionalAccept, sizeof( BOOL ) );
9.如果在发送数据的过程中send()没有完成，还有数据没发送，而调用了close(socket),以前一般采取的措施是shutdown(s,SD_BOTH),但是数
	据将会丢失。
	某些具体程序要求待未发送完的数据发送出去后再关闭socket，可通过设置让程序满足要求：
	struct linger {
	u_short l_onoff;
	u_short l_linger;
	};
	struct linger m_sLinger;
	m_sLinger.l_onoff = 1; //在调用close(socket)时还有数据未发送完，允许等待
	// 若m_sLinger.l_onoff=0;则调用closesocket()后强制关闭
	m_sLinger.l_linger = 5; //设置等待时间为5秒
	setsockopt( s, SOL_SOCKET, SO_LINGER, (const char*)&m_sLinger, sizeof(struct linger));
*/