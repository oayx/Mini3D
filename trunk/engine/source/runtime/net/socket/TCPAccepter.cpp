#include "TCPAccepter.h"
#include "TCPChannel.h"
#include "runtime/thread/Thread.h"
#include "runtime/thread/ThreadScheduler.h"
#if defined(DC_PLATFORM_WIN32)
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(TCPAccepter, Socket);
bool TCPAccepter::Start(const String& hostName, int port, int backlog)
{
	int64 socket;
	int err = Socket::CreateSocket(socket, AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (err)return false;
	
	base::SetSocket(socket);
	err = base::Bind(hostName.c_str(), port);
	if (err)return false;
	
	err = base::Listen(backlog);
	if (err)return false;

	base::SetNonBlocking(true);
	Socket::SetNoDelay(socket, true);
	Socket::SetBufferSize(socket, 65536, 65536);

	Task taskAccept;
	taskAccept.pools = false;
	taskAccept.job = CALLBACK_0(TCPAccepter::HandleAccept, this);
	Thread::Start(taskAccept);

	_isClose = false;
	Debuger::Log("TCPAccepter Start");
	return true;
}
void TCPAccepter::CloseClient(int64 socket)
{
	Socket::CloseSocket(socket);
	this->DoClientClosed(socket);
}
TCPChannel* TCPAccepter::GetClient(int64 socket)
{
	auto it = _channels.Find(socket);
	if (it != _channels.end())
	{
		return it->second;
	}
	return nullptr;
}
void* TCPAccepter::HandleAccept()
{
	while (!_isClose)
	{
		int64 client_socket;
		sockaddr_in remoteAddr;
		int len = base::Accept(&client_socket, 0, &remoteAddr);
		if (len == 0)
		{
			DoAccept(client_socket, remoteAddr);
		}
	}
	return nullptr;
}
void TCPAccepter::DoAccept(int64 socket, const sockaddr_in& remoteAddr)
{
	//先移除，可能存在socket重用
	if (_channels.Find(socket) != _channels.end())
	{
		DoClientClosed(socket);
	}

	//再添加
	LOCK(ThreadScheduler::GlobalMutex);
	TCPChannel* channel = TCPChannel::Create(this, socket);
	_channels.Add(socket, channel);
	if (_acceptCallback != nullptr)_acceptCallback(socket);
}
void TCPAccepter::DoReceive(int64 socket, byte* by, int len)
{
	Debuger::Log("recv:%d", len);
	LOCK(ThreadScheduler::GlobalMutex);
	if (_receiveCallback != nullptr)_receiveCallback(socket, by, len);
}
void TCPAccepter::DoClosed()
{
	LOCK(ThreadScheduler::GlobalMutex);
	for (auto channel : _channels)
	{
		SAFE_DELETE(channel.second);
	}
	_channels.Clear();
	if (_closeCallback != nullptr)_closeCallback();
}
void TCPAccepter::DoClientClosed(int64 socket)
{
	LOCK(ThreadScheduler::GlobalMutex);
	auto it = _channels.Find(socket);
	if (it != _channels.end())
	{
		SAFE_DELETE(it->second);
		_channels.Remove(it);
	}
	if (_clientCloseCallback != nullptr)_clientCloseCallback(socket);
}
void TCPAccepter::DoSocketError(int64 socket, int code)
{
	LOCK(ThreadScheduler::GlobalMutex);
	if (_errorCallback != nullptr)_errorCallback(socket, code);
}
DC_END_NAMESPACE