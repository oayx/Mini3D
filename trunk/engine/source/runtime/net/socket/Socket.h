
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：套接字基础类
*****************************************************************************/
#pragma once

#include "core/Object.h"

struct sockaddr_in;
struct in_addr;
struct addrinfo;

DC_BEGIN_NAMESPACE
#define SendTimeout		1000
#define RecvTimeout		1000
#define SendRecvMaxSize 4096
/********************************************************************/
class ENGINE_DLL Socket : public Object
{
	friend class TCPChannel;
	DISALLOW_COPY_ASSIGN(Socket);
	BEGIN_DERIVED_REFECTION_TYPE(Socket, Object)
	END_DERIVED_REFECTION_TYPE;

protected:
	Socket();
	virtual ~Socket();

	//绑定到指定地址
	//操作成功返回0，否则返回系统错误号
	virtual int Bind(const sockaddr_in* addr);
	virtual int Bind(const char* host_name, int port);

	//开始监听
	//操作成功返回0，否则返回系统错误号
	virtual int Listen(int backlog);

	//发起连接
	//操作成功返回0，否则返回系统错误号；
	//对于非阻塞模式返回0并不表示已经建立连接，而是应当监听SocketEvent::CONNECTED事件
	//或SocketEvent::ERROR事件以确认连接成功或失败。
	virtual int Connect(const addrinfo *addr);
	virtual int Connect(const sockaddr_in *addr);
	virtual int Connect(const char* host_name, int port);

	//接收新的连接
	//参数wait_msec表示最大等待毫秒数；
	//函数返回0表示成功且socket被接收的套接字填充，返回SOCKET_ERROR-1表示等待超时，
	//否则函数返回socket错误号
	virtual int Accept(int64 *socket, int wait_msec, sockaddr_in *remoteAddr);

	//读取套接字内容
	//返回0表示连接已断开，返回SOCKET_ERROR表示发生错误，返回SOCKET_ERROR-1表示没
	//有数据可读(仅对于非阻塞模式套接字)，否则表示实际接收的字节数。
	virtual int Recv(void* buf, int len, const int flags = 0);
	//客户端读取消息
	virtual int Recv(int64 socket, void* buf, int len, const int flags = 0);

	//向套接字写入内容
	//返回0表示连接已断开，返回SOCKET_ERROR表示发生错误，返回SOCKET_ERROR-1表示send
	//操作会发生阻塞且没有数据被发送(仅对于非阻塞模式套接字)，否则返回发送的字节数。
	virtual int Send(const void* buf, int len, const int flags = 0);

	//发送广播数据(适用于SOCK_DGRAM
	//通常建议一个广播数据报的大小不要大到以致产生碎片，也就是说数据报的数据部分（包括头）不超过512字节。
	virtual int RecvFrom(const char* host_name, int port, char* buf, int len, const int flags = 0);
	virtual int SendTo(const char* host_name, int port, const char* buf, int len, const int flags = 0);

	//关闭连接
	virtual void Close();
	virtual void ClientClose(int64 socket);

	virtual void DoConnected() {}
	virtual void DoReceive(int64 socket, byte* by, int len) {}
	virtual void DoClosed() {}
	virtual void DoClientClosed(int64 socket) {}
	virtual void DoSocketError(int64 socket, int code) {}

public:
	//原始套接字句柄
	int64 GetSocket() const { return m_nSocket; }
	void SetSocket(int64 socket) { m_nSocket = socket; }

	//非阻塞模式属性
	//操作成功返回0，否则返回系统错误号
	bool NonBlocking() const { return _isNonBlocking; }
	int  SetNonBlocking(bool nonBlocking);

public:
	//获取接收和发送缓冲大小
	//操作成功返回0，否则返回系统错误号
	static int GetBufferSize(int64 socket, uint &recvSize, uint &sendSize);
	//设置接收和发送缓冲大小
	//每个TCP socket在内核中都有一个发送缓冲区和一个接收缓冲区，TCP的全双工的工作模式以及TCP的流量(拥塞)控制便是依赖于这两个独立的buffer以及buffer的填充状态。
	//1.接收端冲区
	//	接收缓冲区把数据缓存入内核，应用进程一直没有调用recv()进行读取的话，此数据会一直缓存在相应socket的接收缓冲区内。不管进程是否调用recv()读取socket，对端发来的数据都会经由内核接收并且缓存到socket的内核接收缓冲区之中。recv()，就是把内核缓冲区中的数据拷贝到应用层用户的buffer里面，并返回；
	//2.发送缓冲区
	//	进程调用send()发送的数据的时候，最简单情况（也是一般情况），将数据拷贝进入socket的内核发送缓冲区之中，然后send便会在上层返回。换句话说，send（）返回之时，数据不一定会发送到对端去。
	//对于TCP，如果应用进程一直没有读取，接收缓冲区满了之后，发生的动作是：接收端通知发发端，接收窗口关闭。保证TCP套接口接收缓冲区不会溢出，从而保证了TCP是可靠传输。因为对方不允许发出超过大小的数据。 这就是TCP的流量控制，如果对方无视大小而发出了超过大小的数据，则接收方TCP将丢弃它。
	//实际测试：
	//1.如果是阻塞模式发送，对方接收缓冲区满的情况下，发送会阻塞进程，直到对方缓冲区腾出位置
	//2.本机测试默认是64k(win10)，网上查询(win7 8k, winserver2003和2008 8k, winserver2012 64k)
	//3.如果不设置缓冲区大小，客户端一直发送而服务器不接收，导致客户端send一直不返回(像是已经卡死)
	static int SetBufferSize(int64 socket, uint recvSize, uint sendSize);

	//读写超时(毫秒)
	static int SetSendRecvTimeout(int64 socket, uint recvTime, uint sendTime);

	//阻塞模式
	static int SetNonBlocking(int64 socket, bool nonBlocking);

	//是否屏蔽Nagle算法(默认屏蔽true)
	static int SetNoDelay(int64 socket, bool nodelay);

	//close模式(秒)
	static int SetLinger(int64 socket, bool enable, ushort time);

	//广播模式
	static int SetBroadcast(int64 socket);

	//是否开启端口重用
	static int SetReuseAddr(int64 socket, bool enable);

	//创建套接字
	//返回值为socket错误号，0表示成功且socket的内容被填充为新建立的socket。
	//默认值分别是：AF_INET,SOCK_STREAM,IPPROTO_TCP
	static int CreateSocket(int64& socket, const int af = 2, const int type = 1, const int protocol = 6);
	static int CloseSocket(int64 socket);

	//解析域名地址
	//解析域名成功则函数返回0并向addr存储解析后的地址，否则函数返回SOCKET错误码。
	static int ResolveHost(const char* name, in_addr *addr);
	static int ResolveHostV6(const char* name, addrinfo** result);

protected:
	int64	m_nSocket = -1;			//套接字句柄
	bool	_isClose = true;
	bool	_isNonBlocking = false;	//是否非阻塞模式
};
DC_END_NAMESPACE
