#include "UDPRecv.h"
#include "runtime/thread/Thread.h"
#include "runtime/thread/ThreadScheduler.h"
#if defined(DC_PLATFORM_WIN32)
#include <ws2tcpip.h>
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
#include <sys/socket.h>
#include <netdb.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UDPRecv, Socket);
bool UDPRecv::Start(const String& host_name, int port, bool reuse_addr)
{
	_hostName = host_name;
	_port = port;

	//create
	int64 socket;
	int err = Socket::CreateSocket(socket, AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (err)return false;
	base::SetSocket(socket);
	base::SetNonBlocking(true);
	Socket::SetBroadcast(socket);
	if (reuse_addr)Socket::SetReuseAddr(socket, true);

	//bind
	struct sockaddr_in addrIn;
	addrIn.sin_addr.s_addr = ::htonl(INADDR_ANY);
	addrIn.sin_port = ::htons(port);
	err = base::Bind(&addrIn);

	//recv
	Task task_accept;
	task_accept.pools = false;
	task_accept.job = CALLBACK_0(UDPRecv::HandleRecv, this);
	Thread::Start(task_accept);

	_isClose = false;
	Debuger::Log("UDPRecv Start");
	return true;
}
void* UDPRecv::HandleRecv()
{
	while (!_isClose)
	{
		memset(_recvBuffer, 0, sizeof(_recvBuffer));
		int len = base::RecvFrom(_hostName.c_str(), _port, _recvBuffer, sizeof(_recvBuffer));
		if (len > 0)
		{
			thread_lock(ThreadScheduler::GlobalMutex);
			if (_receiveCallback != nullptr)_receiveCallback(_recvBuffer, len);
		}
	}
	return nullptr;
}
DC_END_NAMESPACE
