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
		m_tcpSocket = nullptr;
		m_udpSocket = nullptr;

		m_port = 0;
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

		ErrorCode errorCode;

#ifdef _WIN32
		errorCode = core::WSAInitializer::Get()->Init();
		if (errorCode != ErrorCode::Success)
		{
			return errorCode;
		}
		TME_DEBUG_LOG("Server: WSA initialized successfully.");
#endif

		m_port = _port;

		errorCode = startTcpSocket();
		if (errorCode != ErrorCode::Success)
		{
			return errorCode;
		}

		errorCode = startUdpSocket();
		if (errorCode != ErrorCode::Success)
		{
			m_tcpSocket->closeSocket();
			delete m_tcpSocket;
			m_tcpSocket = nullptr;
			return errorCode;
		}

		m_isRunning = true;

		TME_INFO_LOG("Server: Started successfully on port %d.", m_port);
		return ErrorCode::Success;
	}

	ErrorCode Server::Stop()
	{
		if (!m_isRunning)
		{
			TME_DEBUG_LOG("Server: Stop called but server is not running.");
			return ErrorCode::Success;
		}

		if (m_tcpSocket)
		{
			m_tcpSocket->closeSocket();
			delete m_tcpSocket;
			m_tcpSocket = nullptr;
		}
		TME_DEBUG_LOG("Server: TCP socket closed.");

		if (m_udpSocket)
		{
			m_udpSocket->closeSocket();
			delete m_udpSocket;
			m_udpSocket = nullptr;
		}
		TME_DEBUG_LOG("Server: UDP socket closed.");

#ifdef _WIN32
		core::WSAInitializer::Get()->CleanUp();
		TME_DEBUG_LOG("Server: WSA cleaned up successfully.");
#endif

		TME_INFO_LOG("Server: Stopped successfully.");
		return ErrorCode::Success;
	}

	bool Server::isRunning() const
	{
		return m_isRunning;
	}

	ErrorCode Server::startTcpSocket()
	{
		m_tcpSocket = new core::TcpSocket;

		std::pair<ErrorCode, int> pairResult;

		pairResult = m_tcpSocket->bindSocket(m_port);
		if (pairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("Server: Failed to bind TCP socket on port %d. ErrorCode: %d", m_port, static_cast<int>(pairResult.first));
			delete m_tcpSocket;
			m_tcpSocket = nullptr;
			return pairResult.first;
		}

		pairResult = m_tcpSocket->listenSocket();
		if (pairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("Server: Failed to listen on TCP socket. ErrorCode: %d", static_cast<int>(pairResult.first));
			delete m_tcpSocket;
			m_tcpSocket = nullptr;
			return pairResult.first;
		}

		pairResult = m_tcpSocket->setBlocking(false);
		if (pairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("Server: Failed to set TCP socket to non-blocking mode. ErrorCode: %d", static_cast<int>(pairResult.first));
			delete m_tcpSocket;
			m_tcpSocket = nullptr;
			return pairResult.first;
		}

		TME_DEBUG_LOG("Server: Successfully started TCP socket on port %d.", m_port);
		return ErrorCode::Success;
	}

	ErrorCode Server::startUdpSocket()
	{
		m_udpSocket = new core::UdpSocket;

		std::pair<ErrorCode, int> pairResult;

		pairResult = m_udpSocket->bindSocket(m_port);
		if (pairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("Server: Failed to bind UDP socket on port %d. ErrorCode: %d", m_port, static_cast<int>(pairResult.first));
			delete m_udpSocket;
			m_udpSocket = nullptr;
			return pairResult.first;
		}

		pairResult = m_udpSocket->setBlocking(false);
		if (pairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("Server: Failed to set UDP socket to non-blocking mode. ErrorCode: %d", static_cast<int>(pairResult.first));
			delete m_udpSocket;
			m_udpSocket = nullptr;
			return pairResult.first;
		}

		TME_DEBUG_LOG("Server: Successfully started UDP socket on port %d.", m_port);
		return ErrorCode::Success;
	}
}