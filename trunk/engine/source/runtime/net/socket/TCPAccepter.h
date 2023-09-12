
/*****************************************************************************
* Author： hannibal
* Date：2021/1/18
* Description：tcp服务器
*****************************************************************************/
#pragma once

#include "Socket.h"

DC_BEGIN_NAMESPACE
class TCPChannel;
/********************************************************************/
class ENGINE_DLL TCPAccepter Final : public Socket
{
	typedef Map<int64, TCPChannel*> Channels;
	typedef std::function<void(int64)> AcceptCallback;
	typedef std::function<void(int64, byte*, int)> ReceiveCallback;
	typedef std::function<void()> CloseCallback;
	typedef std::function<void(int64)> ClientCloseCallback;
	typedef std::function<void(int64, int)> ErrorCallback;
	DEFAULT_CREATE(TCPAccepter);
	FRIEND_CONSTRUCT_DESTRUCT(TCPAccepter);
	DISALLOW_COPY_ASSIGN(TCPAccepter);
	BEGIN_DERIVED_REFECTION_TYPE(TCPAccepter, Socket)
	END_DERIVED_REFECTION_TYPE;

public:
	TCPAccepter();
	~TCPAccepter();

	bool Start(const String& host_name, int port, int backlog = 10);	

	void CloseClient(int64 socket);//外部主动关闭
	TCPChannel* GetClient(int64 socket);

	void OnAccept(AcceptCallback callback) { _acceptCallback = callback; }
	void OnReceive(ReceiveCallback callback) { _receiveCallback = callback; }
	void OnClose(CloseCallback callback) { _closeCallback = callback; }
	void OnClientClose(ClientCloseCallback callback) { _clientCloseCallback = callback; }
	void OnError(ErrorCallback callback) { _errorCallback = callback; }

	virtual int  Recv(void* buf, int len, const int flags = 0)override { return base::Recv(buf, len, flags); }
	virtual int  Recv(int64 socket, void* buf, int len, const int flags = 0)override { return base::Recv(socket, buf, len, flags); }
	virtual int  Send(const void* buf, int len, const int flags = 0)override { return base::Send(buf, len, flags); }
	virtual void Close()override { base::Close(); }

private:
	void* HandleAccept();
	void  DoAccept(int64 socket, const sockaddr_in& remoteAddr);
	virtual void DoReceive(int64 socket, byte* by, int len)override;
	virtual void DoClosed()override;
	virtual void DoClientClosed(int64 socket)override;
	virtual void DoSocketError(int64 socket, int code)override;

private:
	AcceptCallback		_acceptCallback;
	ReceiveCallback		_receiveCallback;
	CloseCallback		_closeCallback;
	ClientCloseCallback	_clientCloseCallback;
	ErrorCallback		_errorCallback;

	Channels			_channels;
};
DC_END_NAMESPACE
