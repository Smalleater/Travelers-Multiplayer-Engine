#include "TRA/server/server.hpp"

#include "TRA/debugUtils.hpp"
#include "TRA/engine/connectionStatusComponent.hpp"
#include "TRA/engine/message.hpp"

namespace tra::server
{
	Server* Server::m_singleton = nullptr;

	Server::Server()
	{
		m_networkEngine = new engine::NetworkEngine();
	}

	Server::~Server()
	{
		Stop();
	}

	Server* Server::Get()
	{
		if (m_singleton == nullptr)
		{
			m_singleton = new Server();
		}

		return m_singleton;
	}

	ErrorCode Server::Start(uint16_t _port)
	{
		if (isRunning())
		{
			TRA_DEBUG_LOG("Server: Start called but server is already running.");
			return ErrorCode::ServerAlreadyStarted;
		}

		if (!m_networkEngine)
		{
			TRA_ERROR_LOG("Server: Network engine is not initialized.");
			return ErrorCode::NetworkEngineNotInitialized;
		}

		ErrorCode ec;

		ec = m_networkEngine->startTcpListenOnPort(_port, false);
		if (ec != ErrorCode::Success)
		{
			return ec;
		}

		/*ec = m_networkEngine->startUdpOnPort(_port, false);
		if (ec != ErrorCode::Success)
		{
			m_networkEngine->stopTcpListen();
			return ec;
		}*/

		TRA_INFO_LOG("Server: Started successfully on port %d.", _port);
		return ErrorCode::Success;
	}

	ErrorCode Server::Stop()
	{
		if (!isRunning())
		{
			TRA_DEBUG_LOG("Server: Stop called but server is not running.");
			return ErrorCode::Success;
		}

		if (!m_networkEngine)
		{
			TRA_ERROR_LOG("Server: Network engine is not initialized.");
			return ErrorCode::NetworkEngineNotInitialized;
		}

		ErrorCode ecTcp = m_networkEngine->stopTcpListen();
		//ErrorCode ecUdp = m_networkEngine->stopUdp();

		if (ecTcp != ErrorCode::Success /*|| ecUdp != ErrorCode::Success*/)
		{
			TRA_ERROR_LOG("Server: Failed to stop server sockets properly. TCP ErrorCode: %d", static_cast<int>(ecTcp));
			return ErrorCode::DisconnectWithErrors;
		}
		else
		{
			TRA_INFO_LOG("Server: Stopped successfully.");
			return ErrorCode::Success;
		}
	}

	bool Server::isRunning() const
	{
		return m_networkEngine->entityHasComponent<engine::ListeningComponentTag>(m_networkEngine->getSelfEntityId());
	}

	void Server::beginUpdate()
	{
		if (!isRunning())
		{
			TRA_ERROR_LOG("Server: Cannot begin update, server is not running.");
			return;
		}

		m_networkEngine->beginUpdate();
	}

	void Server::endUpdate()
	{
		if (!isRunning())
		{
			TRA_ERROR_LOG("Server: Cannot end update, server is not running.");
			return;
		}

		m_networkEngine->endUpdate();
	}

	ErrorCode Server::sendTcpMessage(engine::EntityId _entityId, std::shared_ptr<engine::Message> _message)
	{
		if (!isRunning())
		{
			TRA_ERROR_LOG("Server: Cannot send TCP message, server is not running.");
			return ErrorCode::ServerNotRunning;
		}

		return m_networkEngine->sendTcpMessage(_entityId, _message);
	}

	std::pair<ErrorCode, std::vector<std::shared_ptr<engine::Message>>> Server::getTcpMessages(EntityId _entityId, const std::string& _messageType)
	{
		if (!isRunning())
		{
			TRA_ERROR_LOG("Server: Cannot send TCP message, server is not running.");
			return { ErrorCode::ServerNotRunning, {} };
		}

		return m_networkEngine->getTcpMessages(_entityId, _messageType);
	}

	EntityId Server::getSelfEntityId()
	{
		return m_networkEngine->getSelfEntityId();
	}
}