#include "TME/server/server.hpp"

#include "TME/debugUtils.hpp"

#ifdef _WIN32
#include "TME/core/wsaInitializer.hpp"
#endif

namespace tme::server
{
	Server* Server::m_singleton = nullptr;

	Server::Server()
	{
		m_networkEngine = new engine::NetworkEngine();
		m_isRunning = false;
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
		if (m_isRunning)
		{
			TME_DEBUG_LOG("Server: Start called but server is already running.");
			return ErrorCode::ServerAlreadyStarted;
		}

		if (!m_networkEngine)
		{
			TME_ERROR_LOG("Server: Network engine is not initialized.");
			return ErrorCode::NetworkEngineNotInitialized;
		}

		ErrorCode ec = m_networkEngine->startTcpListenOnPort(_port, false);
		if (ec != ErrorCode::Success)
		{
			TME_ERROR_LOG("Server: Failed to start TCP listen on port %d. ErrorCode: %d", _port, static_cast<int>(ec));
			return ec;
		}

		m_isRunning = true;

		TME_INFO_LOG("Server: Started successfully on port %d.", _port);
		return ErrorCode::Success;
	}

	ErrorCode Server::Stop()
	{
		if (!m_isRunning)
		{
			TME_DEBUG_LOG("Server: Stop called but server is not running.");
			return ErrorCode::Success;
		}

		if (!m_networkEngine)
		{
			TME_ERROR_LOG("Server: Network engine is not initialized.");
			return ErrorCode::NetworkEngineNotInitialized;
		}

		ErrorCode ec = m_networkEngine->stopTcpListen();
		if (ec != ErrorCode::Success)
		{
			TME_ERROR_LOG("Server: Failed to stop TCP listen. ErrorCode: %d", static_cast<int>(ec));
			return ec;
		}

		m_isRunning = false;

		TME_INFO_LOG("Server: Stopped successfully.");
		return ErrorCode::Success;
	}

	bool Server::isRunning() const
	{
		return m_isRunning;
	}
}