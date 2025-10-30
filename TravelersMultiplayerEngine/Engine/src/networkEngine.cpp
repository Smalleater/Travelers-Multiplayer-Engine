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
			TME_ERROR_LOG("NetworkEngine: TCP listen socket is already open.");
			return ErrorCode::SocketAlreadyOpen;
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

	ErrorCode NetworkEngine::startTcpConnectToAddress(const std::string& _address, uint16_t _port, bool _blocking)
	{
		if (m_tcpConnectSocket)
		{
			TME_ERROR_LOG("NetworkEngine: TCP connect socket is already open.");
			return ErrorCode::SocketAlreadyOpen;
		}

#ifdef _WIN32
		ErrorCode errorCode = core::WSAInitializer::Get()->Init();
		if (errorCode != ErrorCode::Success)
		{
			return errorCode;
		}

		TME_DEBUG_LOG("NetworkEngine: WSA initialized successfully.");
#endif

		m_tcpConnectSocket = new core::TcpSocket();

		std::pair<ErrorCode, int> intPairResult;

		intPairResult = m_tcpConnectSocket->connectTo(_address, _port);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to connect TCP socket to %s:%d. ErrorCode: %d", _address.c_str(), _port, static_cast<int>(intPairResult.first));
			delete m_tcpConnectSocket;
			m_tcpConnectSocket = nullptr;
			return intPairResult.first;
		}

		intPairResult = m_tcpConnectSocket->setBlocking(_blocking);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to set TCP connect socket blocking mode. ErrorCode: %d", static_cast<int>(intPairResult.first));
			delete m_tcpConnectSocket;
			m_tcpConnectSocket = nullptr;
			return intPairResult.first;
		}

		TME_DEBUG_LOG("NetworkEngine: TCP connect socket connected to %s:%d.", _address.c_str(), _port);
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

	ErrorCode NetworkEngine::stopTcpConnect()
	{
		if (!m_tcpConnectSocket)
		{
			TME_DEBUG_LOG("NetworkEngine: TCP connect socket is not open.");
			return ErrorCode::Success;
		}

		m_tcpConnectSocket->closeSocket();
		delete m_tcpConnectSocket;
		m_tcpConnectSocket = nullptr;

#ifdef _WIN32
		core::WSAInitializer::Get()->CleanUp();
		TME_DEBUG_LOG("NetworkEngine: WSA cleaned up successfully.");
#endif

		TME_DEBUG_LOG("NetworkEngine: TCP connect socket stopped.");
		return ErrorCode::Success;
	}
}