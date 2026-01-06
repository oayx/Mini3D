#include "UDPSend.h"
#include "runtime/thread/Thread.h"
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
IMPL_DERIVED_REFECTION_TYPE(UDPSend, Socket);
UDPSend::UDPSend(const String& hostName, int port)
	: _hostName(hostName), _port(port)
{
	int64 socket;
	int err = Socket::CreateSocket(socket, AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (err == 0)
	{
		base::SetSocket(socket);
		base::SetNonBlocking(true);
		Socket::SetBroadcast(socket);
	}
}
int UDPSend::SendSync(char* buf, int len, int flags)
{
	return base::SendTo(_hostName, _port, buf, len, flags);
}
int UDPSend::SendAsync(char* buf, int len, int flags)
{
	Task task;
	task.job = [this, buf, len, flags]
	{
		base::SendTo(_hostName, _port, buf, len, flags);
		return nullptr;
	};
	Thread::Start(task);
	return 0;
}
DC_END_NAMESPACE
