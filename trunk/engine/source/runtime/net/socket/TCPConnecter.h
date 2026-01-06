
/*****************************************************************************
* Author： hannibal
* Date：2021/1/18
* Description：tcp客户端
*****************************************************************************/
#pragma once

#include "Socket.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL TCPConnecter final : public Socket
{
	typedef std::function<void()> ConnectedCallback;
	typedef std::function<void(byte*, int)> ReceiveCallback;
	typedef std::function<void()> CloseCallback;
	typedef std::function<void(int)> ErrorCallback;
	DEFAULT_CREATE(TCPConnecter);
	FRIEND_CONSTRUCT_DESTRUCT(TCPConnecter);
	DISALLOW_COPY_ASSIGN(TCPConnecter);
	BEGIN_DERIVED_REFECTION_TYPE(TCPConnecter, Socket)
	END_REFECTION_TYPE;

public:
	TCPConnecter() = default;
	~TCPConnecter() = default;

	int Connect(const String& hostName, int port)override;
	void ConnectAsync(const String& hostName, int port);

	void OnConnected(ConnectedCallback callback) { _connectedCallback = callback; }
	void OnReceive(ReceiveCallback callback) { _receiveCallback = callback; }
	void OnClose(CloseCallback callback) { _closeCallback = callback; }
	void OnError(ErrorCallback callback) { _errorCallback = callback; }

	//是否已连接
	bool Connected() const { return _isConnected; }

	virtual int  Recv(void* buf, int len, const int flags = 0)override { return base::Recv(buf, len, flags); }
	virtual int  Send(const void* buf, int len, const int flags = 0)override { return base::Send(buf, len, flags); }
	virtual void Close()override { base::Close(); }

private:
	void* HandleRecv();
	virtual void DoConnected()override;
	virtual void DoReceive(int64 socket, byte* by, int len)override;
	virtual void DoClosed()override;
	virtual void DoSocketError(int64 socket,int code)override;

private:
	ConnectedCallback	_connectedCallback = nullptr;
	ReceiveCallback		_receiveCallback = nullptr;
	CloseCallback		_closeCallback = nullptr;
	ErrorCallback		_errorCallback = nullptr;

	bool				_isConnected = false;		//是否已建立连接
	byte				_recvBuffer[SendRecvMaxSize] = {0};
};
DC_END_NAMESPACE
