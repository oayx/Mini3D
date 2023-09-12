 #pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestSocket : public TestBase
{
	typedef TestBase base;

	TCPConnecter* tcp_connect_socket = nullptr;
	TCPAccepter* tcp_accepter_socket = nullptr;
	UDPSend* udp_send_socket = nullptr;
	UDPRecv* udp_recv_socket = nullptr;
public:
	TestSocket()
	{
		std::function<void()> fun = CALLBACK_0(TestSocket::TestTCPClient, this);
		_menus.Add(std::make_pair("TCPClient", fun));

		fun = CALLBACK_0(TestSocket::TestTCPServer, this);
		_menus.Add(std::make_pair("TCPServer", fun));

		fun = CALLBACK_0(TestSocket::TestUDPSend, this);
		_menus.Add(std::make_pair("UDPSend", fun));

		fun = CALLBACK_0(TestSocket::TestUDPRecv, this);
		_menus.Add(std::make_pair("UDPRecv", fun));
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
	}
	virtual void OnLeave()override 
	{
		SAFE_DELETE(tcp_connect_socket);
		SAFE_DELETE(tcp_accepter_socket);
		SAFE_DELETE(udp_send_socket);
		SAFE_DELETE(udp_recv_socket);
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Socket"; }

private:
	void TestTCPClient()
	{
		tcp_connect_socket = DBG_NEW TCPConnecter();
		tcp_connect_socket->OnConnected([] {Debuger::Log("socket connected"); });
		tcp_connect_socket->ConnectAsync("127.0.0.1", 7003);
		uint timer_id = Timer::AddLoop(1, [this]
			{
				if (tcp_connect_socket != nullptr)
				{
					char buf[] = "12345";
					tcp_connect_socket->Send(buf, 5);
				}
			});
		_timers.Add(timer_id);

		timer_id = Timer::AddOnce(3, [this]
			{
				tcp_connect_socket->Close();
			});
		_timers.Add(timer_id);
	}
	void TestTCPServer()
	{
		tcp_accepter_socket = DBG_NEW TCPAccepter();
		tcp_accepter_socket->Start("127.0.0.1", 7003);
	}
	void TestUDPSend()
	{
		udp_send_socket = DBG_NEW UDPSend("255.255.255.255", 7003);
		uint timer_id = Timer::AddLoop(1, [this]
			{
				char buf[] = "12345";
				udp_send_socket->SendSync(buf, Strlen(buf));
				Debuger::Log("udp send:%s, len:%d", buf, sizeof(buf));
			});
		_timers.Add(timer_id);
	}
	void TestUDPRecv()
	{
		udp_recv_socket = DBG_NEW UDPRecv();
		udp_recv_socket->Start("255.255.255.255", 7003, true);
		udp_recv_socket->OnReceive([](char*by, int len)
			{
				Debuger::Log("msg:%s, len:%d", by, len);
			});
	}
};
DC_END_NAMESPACE