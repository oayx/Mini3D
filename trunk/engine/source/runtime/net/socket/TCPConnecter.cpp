#include "TCPConnecter.h"
#include "runtime/thread/Thread.h"
#include "runtime/thread/ThreadScheduler.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(TCPConnecter, Socket);
int TCPConnecter::Connect(const String& hostName, int port)
{
	_isConnected = false;
	return base::Connect(hostName, port);
}
void TCPConnecter::ConnectAsync(const String& hostName, int port)
{
	_isConnected = false;

	Task task;
	task.job = [this, hostName, port]
	{
		base::Connect(hostName, port);
		return nullptr;
	};
	Thread::Start(task);
}
void* TCPConnecter::HandleRecv()
{
	while (_isConnected)
	{
		int len = base::Recv(_recvBuffer, sizeof(_recvBuffer), 0);
		if (len > 0)
		{
			DoReceive(_nSocket, _recvBuffer, len);
		}
	}
	return nullptr;
}
void TCPConnecter::DoConnected()
{
	base::SetNonBlocking(true);
	Socket::SetNoDelay(_nSocket, true);
	Socket::SetBufferSize(_nSocket, 65536, 65536);

	Task task;
	task.pools = false;
	task.job = CALLBACK_0(TCPConnecter::HandleRecv, this);
	Thread::Start(task);

	{
		LOCK(ThreadScheduler::GlobalMutex);
		_isConnected = false;
		if (_connectedCallback != nullptr)_connectedCallback();
	}
}
void TCPConnecter::DoReceive(int64 socket, byte* by, int len)
{
	if (_receiveCallback != nullptr)
	{
		LOCK(ThreadScheduler::GlobalMutex);
		_receiveCallback(_recvBuffer, len);
	}
}
void TCPConnecter::DoClosed()
{
	LOCK(ThreadScheduler::GlobalMutex);
	_isConnected = false;
	if (_closeCallback != nullptr)_closeCallback();
}
void TCPConnecter::DoSocketError(int64 socket, int code)
{
	if (_errorCallback != nullptr)
	{
		LOCK(ThreadScheduler::GlobalMutex);
		_errorCallback(code);
	}
}
DC_END_NAMESPACE
