#include "TME/engine/networkEngine.hpp"

#include "TME/debugUtils.hpp"

#ifdef _WIN32
#include "TME/core/wsaInitializer.hpp"
#endif

namespace tme::engine
{
	NetworkEngine::NetworkEngine()
	{
		m_tcpLisentSocket = nullptr;
		m_tcpConnectSocket = nullptr;
		m_udpSocket = nullptr;
	}

	NetworkEngine::~NetworkEngine()
	{
		delete m_tcpLisentSocket;
		delete m_tcpConnectSocket;
		delete m_udpSocket;
	}

	ErrorCode NetworkEngine::startTcpListenOnPort(uint16_t _port, bool _blocking)
	{
		if (m_tcpLisentSocket)
		{
			std::pair<ErrorCode, uint16_t> portResult = m_tcpLisentSocket->getPort();
			if (portResult.first != ErrorCode::Success)
			{
				TME_ERROR_LOG("NetworkEngine: Failed to get port of existing TCP listen socket. ErrorCode: %d", static_cast<int>(portResult.first));
				return portResult.first;
			}

			if ( portResult.second == _port)
			{
				TME_DEBUG_LOG("NetworkEngine: TCP listen socket is already open on port %d.", _port);
				return ErrorCode::Success;
			}
			else
			{
				TME_ERROR_LOG("NetworkEngine: TCP listen socket is already open on a different port.");
				return ErrorCode::SocketAlreadyOpen;
			}
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
			delete m_tcpLisentSocket;
			m_tcpLisentSocket = nullptr;
			return intPairResult.first;
		}

		intPairResult = m_tcpLisentSocket->listenSocket();
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to listen on TCP socket. ErrorCode: %d", static_cast<int>(intPairResult.first));
			delete m_tcpLisentSocket;
			m_tcpLisentSocket = nullptr;
			return intPairResult.first;
		}

		intPairResult = m_tcpLisentSocket->setBlocking(_blocking);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to set TCP listen socket blocking mode. ErrorCode: %d", static_cast<int>(intPairResult.first));
			delete m_tcpLisentSocket;
			m_tcpLisentSocket = nullptr;
			return intPairResult.first;
		}

		TME_DEBUG_LOG("NetworkEngine: TCP listen socket started on port %d.", _port);
		return ErrorCode::Success;
	}

	ErrorCode NetworkEngine::stopTcpListen()
	{
		if (!m_tcpLisentSocket)
		{
			TME_DEBUG_LOG("NetworkEngine: TCP listen socket is not open.");
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
}