
/*****************************************************************************
* Author： hannibal
* Date：2021/1/18
* Description：udp接收
* 每个UDP socket都有一个接收缓冲区，没有发送缓冲区，从概念上来说就是只要有数据就发，不管对方是否可以正确接收，所以不缓冲，不需要发送缓冲区。
* 当套接口接收缓冲区满时，新来的数据报无法进入接收缓冲区，此数据报就被丢弃。UDP是没有流量控制的；快的发送者可以很容易地就淹没慢的接收者，导致接收方的UDP丢弃数据报。
*****************************************************************************/
#pragma once

#include "Socket.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL UDPRecv Final : public Socket
{
	typedef std::function<void(char*, int)> ReceiveCallback;
	DEFAULT_CREATE(UDPRecv);
	FRIEND_CONSTRUCT_DESTRUCT(UDPRecv);
	DISALLOW_COPY_ASSIGN(UDPRecv);
	BEGIN_DERIVED_REFECTION_TYPE(UDPRecv, Socket)
	END_DERIVED_REFECTION_TYPE;

public:
	UDPRecv() {}
	~UDPRecv() {}

	bool Start(const String& host_name, int port, bool reuse_addr = false);
	void OnReceive(ReceiveCallback callback) { _receiveCallback = callback; }

	virtual void Close()override { base::Close(); }

private:
	void* HandleRecv();

private:
	String			_hostName = "";
	int				_port = 0;
	char			_recvBuffer[SendRecvMaxSize] = { 0 };
	ReceiveCallback	_receiveCallback = nullptr;
};
DC_END_NAMESPACE
