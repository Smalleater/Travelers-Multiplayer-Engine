#ifndef TME_CLIENT_CLIENT_HPP
#define TME_CLIENT_CLIENT_HPP

#include "TME/export.hpp"

#include <string>
#include <cstdint>

#include "TME/errorCode.hpp"
#include "TME/core/tcpSocket.hpp"
#include "TME/core/udpSocket.hpp"

namespace tme::client
{
	class Client
	{
	public:
		Client(Client& other) = delete;
		void operator=(const Client&) = delete;

		TME_API static Client* Get();

		TME_API ErrorCode ConnectTo(const std::string& _address, uint16_t _port);
		TME_API ErrorCode Disconnect();

	private:
		static Client* m_singleton;

		core::TcpSocket* m_tcpSocket;
		core::UdpSocket* m_udpSocket;

		std::string m_address;
		uint16_t m_tcpPort;
		uint16_t m_udpPort;
		bool m_isConnected;

		Client();
		~Client();

		ErrorCode connectTcpSocket();
		ErrorCode startUdpSocket();
	};
}

#endif
