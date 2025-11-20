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
#include "TME/engine/connectionStatusComponent.hpp"
#include "socketComponent.hpp"
#include "messageComponent.hpp"
#include "selfComponent.hpp"

namespace tme::engine
{
	NetworkEngine::NetworkEngine()
	{
		m_udpSocket = nullptr;

		m_networkEcs = new NetworkEcs();
		NetworkSystemRegistrar::registerNetworkSystems(m_networkEcs);

		m_selfEntityId = m_networkEcs->createEntity();
		TME_ENTITY_ADD_COMPONENT(m_networkEcs, m_selfEntityId, std::make_shared<SelfComponentTag>(), {});
	}

	NetworkEngine::~NetworkEngine()
	{
		delete m_udpSocket;

		m_networkEcs->destroyEntity(m_selfEntityId);

		delete m_networkEcs;
	}

	ErrorCode NetworkEngine::startTcpListenOnPort(uint16_t _port, bool _blocking)
	{
		if (m_networkEcs->hasComponent<TcpListenSocketComponent>(m_selfEntityId))
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

		std::shared_ptr<TcpListenSocketComponent> tcpListenSocketComponent = std::make_shared<TcpListenSocketComponent>();
		tcpListenSocketComponent->m_tcpSocket = new core::TcpSocket();

		std::pair<ErrorCode, int> intPairResult;

		intPairResult = tcpListenSocketComponent->m_tcpSocket->bindSocket(_port);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to bind TCP listen socket on port %d. ErrorCode: %d", _port, static_cast<int>(intPairResult.first));
			stopTcpListen();
			return intPairResult.first;
		}

		intPairResult = tcpListenSocketComponent->m_tcpSocket->listenSocket();
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to listen on TCP socket. ErrorCode: %d", static_cast<int>(intPairResult.first));
			stopTcpListen();
			return intPairResult.first;
		}

		intPairResult = tcpListenSocketComponent->m_tcpSocket->setBlocking(_blocking);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to set TCP listen socket blocking mode. ErrorCode: %d", static_cast<int>(intPairResult.first));
			stopTcpListen();
			return intPairResult.first;
		}

		TME_ENTITY_ADD_COMPONENT(m_networkEcs, m_selfEntityId, tcpListenSocketComponent, {
			TME_INFO_LOG("NetworkEngine: TCP listen socket was not listening on port %d.", _port);
			stopTcpListen();
			return ErrorCode::Failure;
			}
		);

		TME_ENTITY_ADD_COMPONENT(m_networkEcs, m_selfEntityId, std::make_shared<ListeningComponentTag>(), {
			TME_INFO_LOG("NetworkEngine: TCP listen socket was not listening on port %d.", _port);
			stopTcpListen();
			return ErrorCode::Failure;
			}
		);

		TME_DEBUG_LOG("NetworkEngine: TCP listen socket started on port %d.", _port);
		return ErrorCode::Success;
	}

	ErrorCode NetworkEngine::startTcpConnectToAddress(const std::string& _address, uint16_t _port, bool _blocking)
	{
		TME_ASSERT_REF_PTR_OR_COPIABLE(_address);

		if (m_networkEcs->hasComponent<TcpConnectSocketComponent>(m_selfEntityId))
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

		std::shared_ptr<TcpConnectSocketComponent> tcpSocketComponent = std::make_shared<TcpConnectSocketComponent>();
		tcpSocketComponent->m_tcpSocket = new core::TcpSocket();

		std::pair<ErrorCode, int> intPairResult;

		intPairResult = tcpSocketComponent->m_tcpSocket->connectTo(_address, _port);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to connect TCP socket to %s:%d. ErrorCode: %d", _address.c_str(), _port, static_cast<int>(intPairResult.first));
			TME_INFO_LOG("NetworkEngine: TCP connect socket was not connected on port %d.", _port);
			return intPairResult.first;
		}

		intPairResult = tcpSocketComponent->m_tcpSocket->setBlocking(_blocking);
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

		TME_ENTITY_ADD_COMPONENT(m_networkEcs, m_selfEntityId, std::make_shared<NetworkRootComponentTag>(), {
			TME_INFO_LOG("NetworkEngine: TCP connect socket was not connected on port %d.", _port);
			stopTcpListen();
			return ErrorCode::Failure;
			}
		);

		std::shared_ptr<SendTcpMessageComponent> sendMessageComponent = std::make_shared<SendTcpMessageComponent>();
		sendMessageComponent->m_lastMessageByteSent = 0;
		TME_ENTITY_ADD_COMPONENT(m_networkEcs, m_selfEntityId, sendMessageComponent, {
			TME_INFO_LOG("NetworkEngine: TCP connect socket was not connected on port %d.", _port);
			stopTcpListen();
			return ErrorCode::Failure;
			}
		);

		TME_ENTITY_ADD_COMPONENT(m_networkEcs, m_selfEntityId, std::make_shared<ReceiveTcpMessageComponent>(), {
			TME_INFO_LOG("NetworkEngine: TCP connect socket was not connected on port %d.", _port);
			stopTcpListen();
			return ErrorCode::Failure;
			}
		);

		TME_ENTITY_ADD_COMPONENT(m_networkEcs, m_selfEntityId, std::make_shared<ConnectedComponentTag>(), {
			TME_INFO_LOG("NetworkEngine: TCP connect socket was not connected on port %d.", _port);
			stopTcpListen();
			return ErrorCode::Failure;
			}
		);

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
		if (!m_networkEcs->hasComponent<TcpListenSocketComponent>(m_selfEntityId))
		{
			TME_DEBUG_LOG("NetworkEngine: Stop TCP listen called but TCP listen socket is not open.");
			return ErrorCode::Success;
		}

		if (!m_networkEcs->hasComponent<ListeningComponentTag>(m_selfEntityId))
		{
			TME_DEBUG_LOG("NetworkEngine: Stop TCP listen called but ListeningComponentTag is not present on self entity.");
		}
		else
		{
			m_networkEcs->removeComponentFromEntity<ListeningComponentTag>(m_selfEntityId);
		}

		auto getComponentResult = m_networkEcs->getComponentOfEntity<TcpListenSocketComponent>(m_selfEntityId);
		if (getComponentResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to get TcpListentSocketComponent for self entity. ErrorCode: %d", static_cast<int>(getComponentResult.first));
			return getComponentResult.first;
		}

		std::shared_ptr<TcpListenSocketComponent> tcpSocketComponent = getComponentResult.second.lock();
		if (!tcpSocketComponent)
		{
			TME_ERROR_LOG("NetworkEngine: TcpListentSocketComponent for self entity is no longer valid.");
			return ErrorCode::InvalidComponent;
		}

		tcpSocketComponent->m_tcpSocket->shutdownSocket();
		tcpSocketComponent->m_tcpSocket->closeSocket();
		delete tcpSocketComponent->m_tcpSocket;
		tcpSocketComponent->m_tcpSocket = nullptr;

		ErrorCode removeResult = m_networkEcs->removeComponentFromEntity<TcpListenSocketComponent>(m_selfEntityId);
		if (removeResult != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to remove TcpListentSocketComponent from self entity. ErrorCode: %d", static_cast<int>(removeResult));
			return removeResult;
		}

#ifdef _WIN32
		core::WSAInitializer::Get()->CleanUp();
		TME_DEBUG_LOG("NetworkEngine: WSA cleaned up successfully.");
#endif

		TME_DEBUG_LOG("NetworkEngine: TCP listen socket stopped.");
		return ErrorCode::Success;
	}

	ErrorCode NetworkEngine::stopTcpConnect()
	{
		if (!m_networkEcs->hasComponent<TcpConnectSocketComponent>(m_selfEntityId))
		{
			TME_DEBUG_LOG("NetworkEngine: Stop TCP connect called but TCP connect socket is not open.");
			return ErrorCode::Success;
		}

		if (m_networkEcs->hasComponent<ConnectedComponentTag>(m_selfEntityId))
		{
			m_networkEcs->removeComponentFromEntity<ConnectedComponentTag>(m_selfEntityId);
		}
		else
		{
			TME_DEBUG_LOG("NetworkEngine: Stop TCP listen called but ConnectedComponentTag is not present on self entity.");
		}

		auto getComponentResult = m_networkEcs->getComponentOfEntity<TcpConnectSocketComponent>(m_selfEntityId);
		if (getComponentResult.first != ErrorCode::Success)
		{
			std::shared_ptr<TcpConnectSocketComponent> tcpSocketComponent = getComponentResult.second.lock();
			if (tcpSocketComponent)
			{
				tcpSocketComponent->m_tcpSocket->shutdownSocket();
				tcpSocketComponent->m_tcpSocket->closeSocket();
				delete tcpSocketComponent->m_tcpSocket;
				tcpSocketComponent->m_tcpSocket = nullptr;
			}
			else
			{
				TME_ERROR_LOG("NetworkEngine: TcpConnectSocketComponent for self entity is no longer valid.");
			}
		}
		else
		{
			TME_ERROR_LOG("NetworkEngine: Failed to get TcpConnectSocketComponent for self entity. ErrorCode: %d", static_cast<int>(getComponentResult.first));
		}

		ErrorCode removeResult;

		removeResult = m_networkEcs->removeComponentFromEntity<TcpConnectSocketComponent>(m_selfEntityId);
		if (removeResult != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to remove TcpConnectSocketComponent from self entity. ErrorCode: %d", static_cast<int>(removeResult));
		}

		removeResult = m_networkEcs->removeComponentFromEntity<NetworkRootComponentTag>(m_selfEntityId);
		if (removeResult != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to remove NetworkRootComponentTag from self entity. ErrorCode: %d", static_cast<int>(removeResult));
		}

		removeResult = m_networkEcs->removeComponentFromEntity<SendTcpMessageComponent>(m_selfEntityId);
		if (removeResult != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to remove SendTcpMessageComponent from self entity. ErrorCode: %d", static_cast<int>(removeResult));
		}

		removeResult = m_networkEcs->removeComponentFromEntity<ReceiveTcpMessageComponent>(m_selfEntityId);
		if (removeResult != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to remove ReceiveTcpMessageComponent from self entity. ErrorCode: %d", static_cast<int>(removeResult));
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
			TME_ERROR_LOG("NetworkEngine: Failed to get SendTcpMessageComponent for entity %I32u. ErrorCode: %d", _entityId, static_cast<int>(getSendTcpMessageComponentResult.first));
			return getSendTcpMessageComponentResult.first;
		}

		std::shared_ptr<SendTcpMessageComponent> sendTcpMessageComponent = getSendTcpMessageComponentResult.second.lock();
		if (!sendTcpMessageComponent)
		{
			TME_ERROR_LOG("NetworkEngine: SendTcpMessageComponent for entity %I32u is no longer valid.", _entityId);
			return ErrorCode::InvalidComponent;
		}

		sendTcpMessageComponent->m_messagesToSend.push_back(_message);
		return ErrorCode::Success;
	}

	EntityId NetworkEngine::getSelfEntityId()
	{
		return m_selfEntityId;
	}
}