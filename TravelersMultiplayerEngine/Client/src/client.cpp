#include "TME/client/client.hpp"

#include "TME/debugUtils.hpp"

#ifdef _WIN32
#include "TME/core/wsaInitializer.hpp"
#endif

namespace tme::client
{
	Client* Client::m_singleton = nullptr;

	Client::Client()
	{
		m_tcpSocket = nullptr;
		m_udpSocket = nullptr;

		m_address = "";
		m_tcpPort = 0;
		m_udpPort = 0;
		m_isConnected = false;
	}

	Client::~Client()
	{
		Disconnect();
	}

	Client* Client::Get()
	{
		if (m_singleton == nullptr)
		{
			m_singleton = new Client();
		}
		return m_singleton;
	}

	ErrorCode Client::ConnectTo(const std::string& _address, uint16_t _port)
	{
		TME_ASSERT_REF_PTR_OR_COPIABLE(_address);

		if (m_isConnected)
		{
			TME_DEBUG_LOG("Client: ConnectTo called but client is already connected.");
			return ErrorCode::ClientAlreadyConnected;
		}

		ErrorCode errorCode;

#ifdef _WIN32
		errorCode = core::WSAInitializer::Get()->Init();
		if (errorCode != ErrorCode::Success)
		{
			return errorCode;
		}
		TME_DEBUG_LOG("Client: WSA initialized successfully.");
#endif

		m_address = _address;
		m_tcpPort = _port;

		if (isServerLocal(_address))
		{
			m_udpPort = 0;
			TME_DEBUG_LOG("Client: Server is local, UDP port set to 0 for automatic assignment.");
		}
		else
		{
			m_udpPort = _port;
		}

		errorCode = connectTcpSocket();
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

		m_isConnected = true;

		TME_INFO_LOG("Client: Successfully connected to server at %s:%d.", m_address.c_str(), m_tcpPort);
		return ErrorCode::Success;
	}

	ErrorCode Client::Disconnect()
	{
		return ErrorCode::Success;
	}

	bool Client::isServerLocal(const std::string& _address)
	{
		char hostname[256];
		if (gethostname(hostname, sizeof(hostname)))
		{
			return false;
		}

		addrinfo hints = {};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		addrinfo* result = nullptr;
		if (getaddrinfo(hostname, nullptr, &hints, &result))
		{
			return false;
		}

		bool found = false;
		for (addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next)
		{
			char ipstr[INET_ADDRSTRLEN] = {};
			sockaddr_in* sockaddr_ipv4 = reinterpret_cast<sockaddr_in*>(ptr->ai_addr);
			if (inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipstr, sizeof(ipstr)))
			{
				std::string localIp(ipstr);
				if (localIp == _address || _address == "127.0.0.1" || _address == "localhost")
				{
					found = true;
					break;
				}
			}
		}

		freeaddrinfo(result);
		return found;
	}

	ErrorCode Client::connectTcpSocket()
	{
		m_tcpSocket = new core::TcpSocket;

		std::pair<ErrorCode, int> pairResult;

		pairResult = m_tcpSocket->connectTo(m_address, m_tcpPort);
		if (pairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("Client: Failed to connect to %s:%d. ErrorCode: %d", m_address.c_str(), m_tcpPort, static_cast<int>(pairResult.first));
			delete m_tcpSocket;
			m_tcpSocket = nullptr;
			return pairResult.first;
		}

		pairResult = m_tcpSocket->setBlocking(false);
		if (pairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("Client: Failed to set TCP socket to non-blocking mode. ErrorCode: %d", static_cast<int>(pairResult.first));
			delete m_tcpSocket;
			m_tcpSocket = nullptr;
			return pairResult.first;
		}

		TME_DEBUG_LOG("Client: Successfully connected TCP socket to %s:%d.", m_address.c_str(), m_tcpPort);
		return ErrorCode::Success;
	}

	ErrorCode Client::startUdpSocket()
	{
		m_udpSocket = new core::UdpSocket;

		std::pair<ErrorCode, int> pairResult;

		pairResult = m_udpSocket->bindSocket(m_udpPort);
		if (pairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("Client: Failed to bind UDP socket on port %d. ErrorCode: %d", m_udpPort, static_cast<int>(pairResult.first));
			delete m_udpSocket;
			m_udpSocket = nullptr;
			return pairResult.first;
		}

		pairResult = m_udpSocket->setBlocking(false);
		if (pairResult.first != ErrorCode::Success)
		{
			TME_ERROR_LOG("Client: Failed to set UDP socket to non-blocking mode. ErrorCode: %d", static_cast<int>(pairResult.first));
			delete m_udpSocket;
			m_udpSocket = nullptr;
			return pairResult.first;
		}

		TME_DEBUG_LOG("Client: Successfully started UDP socket on port %d.", m_udpPort);
		return ErrorCode::Success;
	}
}