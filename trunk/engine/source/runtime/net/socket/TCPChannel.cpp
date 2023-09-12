#include "TCPChannel.h"
#include "runtime/thread/Thread.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(TCPChannel, Object);
TCPChannel::TCPChannel(Socket* creater, int64 socket)
{
	_isClose = false;
	_creater = creater;
	_socket = socket;

	Task task_recv;
	task_recv.pools = false;
	task_recv.job = CALLBACK_0(TCPChannel::HandleRecv, this);
	Thread::Start(task_recv);
}
TCPChannel::~TCPChannel()
{
	_isClose = true;
	_creater = nullptr;
}
void* TCPChannel::HandleRecv()
{
	while (!_isClose && _creater)
	{
		memset(_recvBuffer, 0, sizeof(_recvBuffer));
		int len = _creater->Recv(_socket, _recvBuffer, sizeof(_recvBuffer), 0);
		if (len > 0)
		{
			_creater->DoReceive(_socket, _recvBuffer, len);
		}
		Thread::Sleep(5000);
	}
	return nullptr;
}
DC_END_NAMESPACE