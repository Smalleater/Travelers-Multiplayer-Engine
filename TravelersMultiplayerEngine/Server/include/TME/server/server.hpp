#ifndef TME_SERVER_SERVER_HPP
#define TME_SERVER_SERVER_HPP

#include "TME/export.hpp"

#include <cstdint>
#include <unordered_map>

#include "TME/errorCode.hpp"
#include "TME/core/tcpSocket.hpp"
#include "TME/core/udpSocket.hpp"

namespace tme::server
{
	struct Client;

	using ClientMap = std::unordered_map<uint32_t, Client*>;

	class Server
	{
	public:
		Server(Server& other) = delete;
		void operator=(const Server&) = delete;

		TME_API static Server* Get();

		TME_API ErrorCode Start(uint16_t _port);
		TME_API ErrorCode Stop();

		TME_API bool isRunning() const;

	private:
		static Server* m_singleton;

		core::TcpSocket* m_tcpSocket;
		core::UdpSocket* m_udpSocket;

		ClientMap clients;

		uint16_t m_port;
		bool m_isRunning;

		Server();
		~Server();

		ErrorCode startTcpSocket();
		ErrorCode startUdpSocket();
	};

	
}

#endif