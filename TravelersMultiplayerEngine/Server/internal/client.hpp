#ifndef TME_SERVER_CLIENT_HPP
#define TME_SERVER_CLIENT_HPP

#include <cstdint>
#include <unordered_map>

#include "TME/core/tcpSocket.hpp"
#include "TME/core/udpSocket.hpp"

namespace tme::server
{
	struct Client
	{
		uint16_t m_networkId;
		core::TcpSocket* m_tcpSocket;
		sockaddr* m_udpSocket;
	};

	using ClientMap = std::unordered_map<uint32_t, Client>;
}

#endif
