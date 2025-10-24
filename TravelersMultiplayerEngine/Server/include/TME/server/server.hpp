#ifndef TME_SERVER_SERVER_HPP
#define TME_SERVER_SERVER_HPP

#include "TME/export.hpp"

#include <cstdint>

#include "TME/errorCode.hpp"
#include "TME/core/tcpSocket.hpp"
#include "TME/core/udpSocket.hpp"

namespace tme::server
{
	class Server
	{
	public:
		Server(Server& other) = delete;
		void operator=(const Server&) = delete;

		TME_API static Server* Get();

		TME_API ErrorCode Start(uint16_t _port);
		TME_API ErrorCode Stop();

	private:
		static Server* m_singleton;

		core::TcpSocket* m_tcpSocket;
		core::UdpSocket* m_udpSocket;
		bool m_isStarted;

		Server();
		~Server();
	};

	
}

#endif