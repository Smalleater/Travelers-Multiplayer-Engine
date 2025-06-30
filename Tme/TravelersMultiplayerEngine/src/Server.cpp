#include "TME/Server.h"

#include "TME/Utils.h"
#include "TME/WinsockInitializer.h"
#include "TME/ServiceLocator.h"

namespace tme
{
	const char* Server::m_port = nullptr;
	addrinfo* Server::m_result = nullptr;
	addrinfo Server::m_hints;
	SOCKET Server::m_listenSocket = INVALID_SOCKET;

	bool Server::start(const char* port)
	{
		if (!Utils::isValidPort(port))
		{
			ServiceLocator::logger().logError("The provided port is not valid");
			return false;
		}

		if (WinsockInitializer::isStarted())
		{
			ServiceLocator::logger().logError("The server has already been started");
			return false;
		}

		if (!WinsockInitializer::start())
		{
			ServiceLocator::logger().logError("An error occurred while initializing Winsock");
			return false;
		}

		m_port = port;

		if (!createSocket())
		{
			ServiceLocator::logger().logError("An error occurred while creating the socket");
			return false;
		}

		if (!bindSocket())
		{
			ServiceLocator::logger().logError("An error occurred while binding the socket");
			return false;
		}

		if (!listenSocket())
		{
			ServiceLocator::logger().logError(static_cast<std::string>("Error occurred while the server was attempting to listen on the port ") + port);
			return false;
		}

		ServiceLocator::logger().logInfo("Server has started successfully");
		ServiceLocator::logger().logInfo(static_cast < std::string>("The server is currently listening on port ") + port);
		return true;
	}

	bool Server::createSocket()
	{
		ZeroMemory(&m_hints, sizeof(m_hints));
		m_hints.ai_family = AF_INET;
		m_hints.ai_socktype = SOCK_STREAM;
		m_hints.ai_protocol = IPPROTO_TCP;
		m_hints.ai_flags = AI_PASSIVE;

		int iResult;

		iResult = getaddrinfo(NULL, m_port, &m_hints, &m_result);
		if (iResult != 0)
		{
			WinsockInitializer::close();
			return false;
		}

		m_listenSocket = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol);
		if (m_listenSocket == INVALID_SOCKET)
		{
			freeaddrinfo(m_result);
			WinsockInitializer::close();
			return false;
		}

		return true;
	}

	bool Server::bindSocket()
	{
		int iResult;

		iResult = bind(m_listenSocket, m_result->ai_addr, (int)m_result->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			freeaddrinfo(m_result);
			WinsockInitializer::close();
			return false;
		}

		freeaddrinfo(m_result);

		return true;
	}

	bool Server::listenSocket()
	{
		if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			WinsockInitializer::close();
			return false;
		}

		return true;
	}
}
