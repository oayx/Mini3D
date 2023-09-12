#include "TCPConnecter.h"
#include "runtime/thread/Thread.h"
#include "runtime/thread/ThreadScheduler.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(TCPConnecter, Socket);
TCPConnecter::TCPConnecter()
{
}
TCPConnecter::~TCPConnecter()
{
}
void TCPConnecter::Connect(const String& host_name, int port)
{
	_isConnected = false;
	base::Connect(host_name.c_str(), port);
}
void TCPConnecter::ConnectAsync(const String& host_name, int port)
{
	_isConnected = false;

	Task task;
	task.job = [this, host_name, port]
	{
		base::Connect(host_name.c_str(), port);
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
			DoReceive(m_nSocket, _recvBuffer, len);
		}
	}
	return nullptr;
}
void TCPConnecter::DoConnected()
{
	base::SetNonBlocking(true);
	Socket::SetNoDelay(m_nSocket, true);
	Socket::SetBufferSize(m_nSocket, 65536, 65536);

	Task task;
	task.pools = false;
	task.job = CALLBACK_0(TCPConnecter::HandleRecv, this);
	Thread::Start(task);

	{
		thread_lock(ThreadScheduler::GlobalMutex);
		_isConnected = false;
		if (_connectedCallback != nullptr)_connectedCallback();
	}
}
void TCPConnecter::DoReceive(int64 socket, byte* by, int len)
{
	if (_receiveCallback != nullptr)
	{
		thread_lock(ThreadScheduler::GlobalMutex);
		_receiveCallback(_recvBuffer, len);
	}
}
void TCPConnecter::DoClosed()
{
	thread_lock(ThreadScheduler::GlobalMutex);
	_isConnected = false;
	if (_closeCallback != nullptr)_closeCallback();
}
void TCPConnecter::DoSocketError(int64 socket, int code)
{
	if (_errorCallback != nullptr)
	{
		thread_lock(ThreadScheduler::GlobalMutex);
		_errorCallback(code);
	}
}
DC_END_NAMESPACE
