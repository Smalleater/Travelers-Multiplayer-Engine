#ifndef TRA_ENGINE_SOCKET_COMPONENT_HPP
#define TRA_ENGINE_SOCKET_COMPONENT_HPP

#include "TRA/engine/iNetworkComponent.hpp"

#include "TRA/core/tcpSocket.hpp"

namespace tra::engine
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
