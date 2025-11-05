#include "TME/engine/networkEngine.hpp"

#include "TME/debugUtils.hpp"
#include "TME/core/netUtils.hpp"

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

		if (m_tcpConnectSocket)
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

		m_tcpConnectSocket = new core::TcpSocket();

		std::pair<ErrorCode, int> intPairResult;

		intPairResult = m_tcpConnectSocket->connectTo(_address, _port);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to connect TCP socket to %s:%d. ErrorCode: %d", _address.c_str(), _port, static_cast<int>(intPairResult.first));
			stopTcpConnect();
			return intPairResult.first;
		}

		intPairResult = m_tcpConnectSocket->setBlocking(_blocking);
		if (intPairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("NetworkEngine: Failed to set TCP connect socket blocking mode. ErrorCode: %d", static_cast<int>(intPairResult.first));
			stopTcpConnect();
			return intPairResult.first;
		}

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
		if (!m_tcpConnectSocket)
		{
			TME_DEBUG_LOG("NetworkEngine: Stop TCP connect called but TCP connect socket is not open.");
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

	EntityId NetworkEngine::createEntity()
	{
		return m_networkEcs.createEntity();
	}
}