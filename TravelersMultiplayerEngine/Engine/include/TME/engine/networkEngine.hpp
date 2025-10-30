#ifndef TME_ENGINE_NETWORK_ENGINE_HPP
#define TME_ENGINE_NETWORK_ENGINE_HPP

#include "TME/export.hpp"

#include <cstdint>

#include "TME/errorCode.hpp"
#include "TME/core/tcpSocket.hpp"
#include "TME/core/udpSocket.hpp"

namespace tme::engine
{
	class NetworkEngine
	{
	public:
		TME_API NetworkEngine();
		TME_API ~NetworkEngine();

		TME_API ErrorCode startTcpListenOnPort(uint16_t _port, bool _blocking);
		TME_API ErrorCode startTcpConnectToAddress(const std::string& _address, uint16_t _port, bool _blocking);
		TME_API ErrorCode startUdpOnPort(uint16_t _port, bool _blocking);

		TME_API ErrorCode stopTcpListen();
		TME_API ErrorCode stopTcpConnect();
		TME_API ErrorCode stopUdp();

	private:
		core::TcpSocket* m_tcpLisentSocket;
		core::TcpSocket* m_tcpConnectSocket;
		core::UdpSocket* m_udpSocket;
	};
}

#endif
