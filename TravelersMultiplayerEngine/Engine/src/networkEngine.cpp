#include "TME/engine/networkEngine.hpp"

#include "TME/debugUtils.hpp"
#include "TME/core/netUtils.hpp"
#include "TME/engine/message.hpp"
#include "TME/engine/networkEcsUtils.hpp"

#ifdef _WIN32
#include "TME/core/wsaInitializer.hpp"
#endif

#include "networkSystemRegistrar.hpp"

#include "TME/engine/networkRootComponentTag.hpp"
#include "tcpSocketComponent.hpp"
#include "messageComponent.hpp"

namespace tme::engine
{
	NetworkEngine::NetworkEngine()
	{
		m_tcpLisentSocket = nullptr;
		m_udpSocket = nullptr;

		m_networkEcs = new NetworkEcs();
		NetworkSystemRegistrar::registerNetworkSystems(m_networkEcs);

		m_selfEntityId = m_networkEcs->createEntity();
		TME_ENTITY_ADD_COMPONENT(m_networkEcs, m_selfEntityId, std::make_shared<NetworkRootComponentTag>(), {});
	}

	NetworkEngine::~NetworkEngine()
	{
		delete m_tcpLisentSocket;
		delete m_udpSocket;

		m_networkEcs->destroyEntity(m_selfEntityId);

		delete m_networkEcs;
	}

	ErrorCode NetworkEngine::startTcpListenOnPort(uint16_t _port, bool _blocking)
	{
		if (m_tcpLisentSocket)
		{
			TME_ERROR_LOG("NetworkEngine: Start TCP listen on port callrd but TCP listen socket is already open.");
			return ErrorCode::SocketAlreadyOpen;
		}

		if (!core::NetUtils::isValidPort(_port))
		{
			TME_ERROR_LOG("NetworkEngine: Invalid port number %d for TCP listen socket.", _port);
			return ErrorCode::InvalidPortNumber;
		}

#ifdef _WIN32
		ErrorCode errorCode = core::WSAInitializer::Get()->Init();
		if (errorCode != ErrorCode::Success)
		{
			return errorCode;
		}

		TME_DEBUG_LOG("NetworkEngine: WSA initialized successfully.");
#endif

		m_tcpLisentSocket = new core::TcpSocket();
		std::pair<ErrorCode, int> intPairResult;

		intPairResult = m_tcpLisentSocket->bindSocket(_port);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to bind TCP listen socket on port %d. ErrorCode: %d", _port, static_cast<int>(intPairResult.first));
			stopTcpListen();
			return intPairResult.first;
		}

		intPairResult = m_tcpLisentSocket->listenSocket();
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to listen on TCP socket. ErrorCode: %d", static_cast<int>(intPairResult.first));
			stopTcpListen();
			return intPairResult.first;
		}

		intPairResult = m_tcpLisentSocket->setBlocking(_blocking);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to set TCP listen socket blocking mode. ErrorCode: %d", static_cast<int>(intPairResult.first));
			stopTcpListen();
			return intPairResult.first;
		}

		TME_DEBUG_LOG("NetworkEngine: TCP listen socket started on port %d.", _port);
		return ErrorCode::Success;
	}

	ErrorCode NetworkEngine::startTcpConnectToAddress(const std::string& _address, uint16_t _port, bool _blocking)
	{
		TME_ASSERT_REF_PTR_OR_COPIABLE(_address);

		if (m_networkEcs->hasComponent<TcpSocketComponent>(m_selfEntityId))
		{
			TME_ERROR_LOG("NetworkEngine: start TCP connect to address called but TCP connect socket is already open.");
			return ErrorCode::SocketAlreadyOpen;
		}

		if (!core::NetUtils::isValidIpV4Address(_address))
		{
			TME_ERROR_LOG("NetworkEngine: Invalid IP address %s for TCP connect socket.", _address.c_str());
			return ErrorCode::InvalidIpAddress;
		}

		if (!core::NetUtils::isValidPort(_port))
		{
			TME_ERROR_LOG("NetworkEngine: Invalid port number %d for TCP listen socket.", _port);
			return ErrorCode::InvalidPortNumber;
		}

#ifdef _WIN32
		ErrorCode errorCode = core::WSAInitializer::Get()->Init();
		if (errorCode != ErrorCode::Success)
		{
			return errorCode;
		}

		TME_DEBUG_LOG("NetworkEngine: WSA initialized successfully.");
#endif

		std::shared_ptr<TcpSocketComponent> tcpSocketComponent = std::make_shared<TcpSocketComponent>();
		tcpSocketComponent->tcpSocket = new core::TcpSocket();

		std::pair<ErrorCode, int> intPairResult;

		intPairResult = tcpSocketComponent->tcpSocket->connectTo(_address, _port);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to connect TCP socket to %s:%d. ErrorCode: %d", _address.c_str(), _port, static_cast<int>(intPairResult.first));
			TME_INFO_LOG("NetworkEngine: TCP connect socket was not connected on port %d.", _port);
			return intPairResult.first;
		}

		intPairResult = tcpSocketComponent->tcpSocket->setBlocking(_blocking);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to set TCP connect socket blocking mode. ErrorCode: %d", static_cast<int>(intPairResult.first));
			TME_INFO_LOG("NetworkEngine: TCP connect socket was not connected on port %d.", _port);
			return intPairResult.first;
		}

		TME_ENTITY_ADD_COMPONENT(m_networkEcs, m_selfEntityId, tcpSocketComponent, {
			TME_INFO_LOG("NetworkEngine: TCP connect socket was not connected on port %d.", _port);
			return ErrorCode::Failure;
			}
		);

		///////////

		/*m_tcpConnectSocket = new core::TcpSocket();
		std::pair<ErrorCode, int> intPairResult;*/

		/*intPairResult = m_tcpConnectSocket->connectTo(_address, _port);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to connect TCP socket to %s:%d. ErrorCode: %d", _address.c_str(), _port, static_cast<int>(intPairResult.first));
			stopTcpConnect();
			return intPairResult.first;
		}*/

		/*intPairResult = m_tcpConnectSocket->setBlocking(_blocking);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to set TCP connect socket blocking mode. ErrorCode: %d", static_cast<int>(intPairResult.first));
			stopTcpConnect();
			return intPairResult.first;
		}*/

		TME_DEBUG_LOG("NetworkEngine: TCP connect socket connected to %s:%d.", _address.c_str(), _port);
		return ErrorCode::Success;
	}

	ErrorCode NetworkEngine::startUdpOnPort(uint16_t _port, bool _blocking)
	{
		if (m_udpSocket)
		{
			TME_ERROR_LOG("NetworkEngine: start UDP on port called but UDP socket is already open.");
			return ErrorCode::SocketAlreadyOpen;
		}

		if (!core::NetUtils::isValidPort(_port))
		{
			TME_ERROR_LOG("NetworkEngine: Invalid port number %d for UDP socket.", _port);
			return ErrorCode::InvalidPortNumber;
		}

#ifdef _WIN32
		ErrorCode errorCode = core::WSAInitializer::Get()->Init();
		if (errorCode != ErrorCode::Success)
		{
			return errorCode;
		}

		TME_DEBUG_LOG("NetworkEngine: WSA initialized successfully.");
#endif

		m_udpSocket = new core::UdpSocket();

		std::pair<ErrorCode, int> intPairResult;

		intPairResult = m_udpSocket->bindSocket(_port);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to bind UDP socket on port %d. ErrorCode: %d", _port, static_cast<int>(intPairResult.first));
			stopUdp();
			return intPairResult.first;
		}

		intPairResult = m_udpSocket->setBlocking(_blocking);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to set UDP socket blocking mode. ErrorCode: %d", static_cast<int>(intPairResult.first));
			stopUdp();
			return intPairResult.first;
		}

		if (_port == 0)
		{
			std::pair<ErrorCode, uint16_t> portResult = m_udpSocket->getPort();

			if (portResult.first != ErrorCode::Success)
			{
				TME_ERROR_LOG("NetworkEngine: Failed to get assigned UDP port number. ErrorCode: %d", static_cast<int>(portResult.first));
				stopUdp();
				return portResult.first;
			}

			_port = portResult.second;
		}

		TME_DEBUG_LOG("NetworkEngine: UDP socket started on port %d.", _port);
		return ErrorCode::Success;
	}

	ErrorCode NetworkEngine::stopTcpListen()
	{
		if (!m_tcpLisentSocket)
		{
			TME_DEBUG_LOG("NetworkEngine: Stop TCP listen called but TCP listen socket is not open.");
			return ErrorCode::Success;
		}

		m_tcpLisentSocket->closeSocket();
		delete m_tcpLisentSocket;
		m_tcpLisentSocket = nullptr;

#ifdef _WIN32
		core::WSAInitializer::Get()->CleanUp();
		TME_DEBUG_LOG("NetworkEngine: WSA cleaned up successfully.");
#endif

		TME_DEBUG_LOG("NetworkEngine: TCP listen socket stopped.");
		return ErrorCode::Success;
	}

	ErrorCode NetworkEngine::stopTcpConnect()
	{
		if (!m_networkEcs->hasComponent<TcpSocketComponent>(m_selfEntityId))
		{
			TME_DEBUG_LOG("NetworkEngine: Stop TCP connect called but TCP connect socket is not open.");
			return ErrorCode::Success;
		}

		auto getComponentResult = m_networkEcs->getComponentOfEntity<TcpSocketComponent>(m_selfEntityId);
		if (getComponentResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to get TcpSocketComponent for self entity. ErrorCode: %d", static_cast<int>(getComponentResult.first));
			return getComponentResult.first;
		}

		std::shared_ptr<TcpSocketComponent> tcpSocketComponent = getComponentResult.second.lock();
		if (!tcpSocketComponent)
		{
			TME_ERROR_LOG("NetworkEngine: TcpSocketComponent for self entity is no longer valid.");
			return ErrorCode::InvalidComponent;
		}
		
		ErrorCode removeResult = m_networkEcs->removeComponentFromEntity<TcpSocketComponent>(m_selfEntityId);
		if (removeResult != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to remove TcpSocketComponent from self entity. ErrorCode: %d", static_cast<int>(removeResult));
			return removeResult;
		}

#ifdef _WIN32
		core::WSAInitializer::Get()->CleanUp();
		TME_DEBUG_LOG("NetworkEngine: WSA cleaned up successfully.");
#endif

		TME_DEBUG_LOG("NetworkEngine: TCP connect socket stopped.");
		return ErrorCode::Success;
	}

	ErrorCode NetworkEngine::stopUdp()
	{
		if (!m_udpSocket)
		{
			TME_DEBUG_LOG("NetworkEngine: Stop UDP called but UDP socket is not open.");
			return ErrorCode::Success;
		}

		m_udpSocket->closeSocket();
		delete m_udpSocket;
		m_udpSocket = nullptr;

#ifdef _WIN32
		core::WSAInitializer::Get()->CleanUp();
		TME_DEBUG_LOG("NetworkEngine: WSA cleaned up successfully.");
#endif

		TME_DEBUG_LOG("NetworkEngine: UDP socket stopped.");
		return ErrorCode::Success;
	}

	void NetworkEngine::beginUpdate()
	{
		ErrorCode errorCode = acceptConnection();
		if (errorCode != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to accept new TCP connections. ErrorCode: %d", static_cast<int>(errorCode));
		}

		m_networkEcs->beginUpdate();
	}

	void NetworkEngine::endUpdate()
	{
		m_networkEcs->endUpdate();
	}

	EntityId NetworkEngine::createEntity()
	{
		return m_networkEcs->createEntity();
	}

	void NetworkEngine::destroyEntity(EntityId _entityId)
	{
		m_networkEcs->destroyEntity(_entityId);
	}

	ErrorCode NetworkEngine::sendTcpMessage(EntityId _entityId, std::shared_ptr<Message> _message)
	{
		auto getSendTcpMessageComponentResult = m_networkEcs->getComponentOfEntity<SendTcpMessageComponent>(_entityId);
		if (getSendTcpMessageComponentResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to get SendTcpMessageComponent for entity %llu. ErrorCode: %d", _entityId, static_cast<int>(getSendTcpMessageComponentResult.first));
			return getSendTcpMessageComponentResult.first;
		}

		std::shared_ptr<SendTcpMessageComponent> sendTcpMessageComponent = getSendTcpMessageComponentResult.second.lock();
		if (!sendTcpMessageComponent)
		{
			TME_ERROR_LOG("NetworkEngine: SendTcpMessageComponent for entity %llu is no longer valid.", _entityId);
			return ErrorCode::InvalidComponent;
		}

		sendTcpMessageComponent->m_messagesToSend.push_back(_message);
		return ErrorCode::Success;
	}

	EntityId NetworkEngine::getSelfEntityId()
	{
		return m_selfEntityId;
	}

	ErrorCode NetworkEngine::acceptConnection()
	{
		uint8_t acceptedConnections = 0;

		core::TcpSocket* clientSocket = nullptr;
		EntityId newEntityId = 0;
		ErrorCode errorCode = ErrorCode::Success;
		std::shared_ptr<NetworkRootComponentTag> networkRootComponentTag = nullptr;
		std::shared_ptr<TcpSocketComponent> tcpSocketComponent = nullptr;
		std::shared_ptr<SendTcpMessageComponent> sendMessageComponent = nullptr;
		std::shared_ptr<ReceiveTcpMessageComponent> receiveMessageComponent = nullptr;

		while (acceptedConnections < MAX_ACCEPTED_CONNECTIONS_PAR_TICK)
		{
			clientSocket = nullptr;
			std::pair<ErrorCode, int> intPairResult = m_tcpLisentSocket->acceptSocket(&clientSocket);
			if (intPairResult.first == ErrorCode::SocketWouldBlock)
			{
				break;
			}
			else if (intPairResult.first != ErrorCode::Success)
			{
				return intPairResult.first;
			}

			newEntityId = createEntity();

			TME_ENTITY_ADD_COMPONENT(m_networkEcs, newEntityId, std::shared_ptr<NetworkRootComponentTag>(), {
				clientSocket->closeSocket();
				delete clientSocket;
				destroyEntity(newEntityId);
				continue;
				});

			tcpSocketComponent = std::make_shared<TcpSocketComponent>();
			tcpSocketComponent->tcpSocket = clientSocket;
			TME_ENTITY_ADD_COMPONENT(m_networkEcs, newEntityId, tcpSocketComponent, {
				clientSocket->closeSocket();
				delete clientSocket;
				destroyEntity(newEntityId);
				continue;
				});

			TME_ENTITY_ADD_COMPONENT(m_networkEcs, newEntityId, std::make_shared<ReceiveTcpMessageComponent>(), {
				clientSocket->closeSocket();
				delete clientSocket;
				destroyEntity(newEntityId);
				continue;
				});

			sendMessageComponent = std::make_shared<SendTcpMessageComponent>();
			sendMessageComponent->m_lastMessageByteSent = 0;
			TME_ENTITY_ADD_COMPONENT(m_networkEcs, newEntityId, sendMessageComponent, {
				clientSocket->closeSocket();
				delete clientSocket;
				destroyEntity(newEntityId);
				continue;
				});

			tcpSocketComponent.reset();

			acceptedConnections++;
			TME_INFO_LOG("Engine: Accepted new TCP connection. Entity ID: %llu", newEntityId);
		}

		return ErrorCode::Success;
	}
}