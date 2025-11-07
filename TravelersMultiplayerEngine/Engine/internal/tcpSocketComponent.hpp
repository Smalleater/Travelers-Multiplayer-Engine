#ifndef TME_ENGINE_TCP_SOCKET_COMPONENT_HPP
#define TME_ENGINE_TCP_SOCKET_COMPONENT_HPP

#include "TME/engine/iNetworkComponent.hpp"

namespace tme::engine
{
	struct TcpSocketComponent : public INetworkComponent
	{
		core::TcpSocket* tcpSocket;

		TcpSocketComponent() : tcpSocket(nullptr) {}

		virtual ~TcpSocketComponent()
		{
			if (tcpSocket)
			{
				tcpSocket->closeSocket();
				delete tcpSocket;
				tcpSocket = nullptr;
			}
		}
	};
}

#endif
