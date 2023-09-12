
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：
*****************************************************************************/
#pragma once

#include "Socket.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL TCPChannel Final : public Object
{
	friend class TCPAccepter;
	DEFAULT_CREATE(TCPChannel);
	FRIEND_CONSTRUCT_DESTRUCT(TCPChannel);
	DISALLOW_COPY_ASSIGN(TCPChannel);
	BEGIN_DERIVED_REFECTION_TYPE(TCPChannel, Object)
	END_DERIVED_REFECTION_TYPE;

	TCPChannel(Socket* creater, int64 socket);
	~TCPChannel();

private:
	void*	HandleRecv();

private:
	Socket* _creater = nullptr;
	int64	_socket = -1;
	bool	_isClose = true;
	byte	_recvBuffer[SendRecvMaxSize] = {0};
};
DC_END_NAMESPACE