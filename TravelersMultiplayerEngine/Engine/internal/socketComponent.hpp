#ifndef TME_ENGINE_SOCKET_COMPONENT_HPP
#define TME_ENGINE_SOCKET_COMPONENT_HPP

#include "TME/engine/iNetworkComponent.hpp"

namespace tme::engine
{
	struct TcpListenSocketComponent : public INetworkComponent
	{
		core::TcpSocket* m_tcpSocket;

		TcpListenSocketComponent() : m_tcpSocket(nullptr) {}

		~TcpListenSocketComponent()
		{
			if (m_tcpSocket)
			{
				m_tcpSocket->shutdownSocket();
				m_tcpSocket->closeSocket();
				delete m_tcpSocket;
			}
		}
	};

	struct TcpConnectSocketComponent : public INetworkComponent
	{
		core::TcpSocket* m_tcpSocket;

		TcpConnectSocketComponent() : m_tcpSocket(nullptr) {}

		~TcpConnectSocketComponent()
		{
			if (m_tcpSocket)
			{
				m_tcpSocket->shutdownSocket();
				m_tcpSocket->closeSocket();
				delete m_tcpSocket;
			}
		}
	};
}

#endif
