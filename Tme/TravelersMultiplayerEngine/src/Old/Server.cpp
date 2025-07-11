#include "TME/Old/Server.h"

#include "TME/Old/Utils.h"
#include "TME/Old/WinsockInitializer.h"

#include "ServiceLocator.h"

namespace tme
{
	addrinfo* Server::m_result = nullptr;
	addrinfo Server::m_hints;
	SOCKET Server::m_listenSocket = INVALID_SOCKET;
	bool Server::m_isRunning = false;
	std::vector<SOCKET> Server::m_clients;

	bool Server::start(const char* port)
	{
		if (m_isRunning)
		{
			ServiceLocator::logger().logError("The server is already running");
		}

		if (!Utils::isValidPort(port))
		{
			ServiceLocator::logger().logError("The provided port is not valid");
			return false;
		}

		if (!WinsockInitializer::isStarted())
		{
			if (!WinsockInitializer::start())
			{
				ServiceLocator::logger().logError("An error occurred while initializing Winsock");
				return false;
			}
		}

		if (!createSocket(port))
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

		m_isRunning = true;

		ServiceLocator::threadManager().addJob([]()
			{
				acceptLoop();
			});

		ServiceLocator::logger().logInfo("Server has started successfully");
		ServiceLocator::logger().logInfo(static_cast <std::string>("The server is currently listening on port ") + port);
		return true;
	}

	bool Server::stop()
	{
		m_isRunning = false;

		for (SOCKET client : m_clients)
		{
			shutdown(client, SD_BOTH);
			closesocket(client);
		}
		m_clients.clear();
		ServiceLocator::logger().logInfo("All clients have been disconnected successfully");

		m_listenSocket = INVALID_SOCKET;

		WinsockInitializer::close();

		ServiceLocator::logger().logInfo("The server has been stopped successfully");

		return true;
	}

	bool Server::isRunning()
	{
		return m_isRunning;
	}

	bool Server::createSocket(const char* port)
	{
		ZeroMemory(&m_hints, sizeof(m_hints));
		m_hints.ai_family = AF_INET;
		m_hints.ai_socktype = SOCK_STREAM;
		m_hints.ai_protocol = IPPROTO_TCP;
		m_hints.ai_flags = AI_PASSIVE;

		int iResult;

		iResult = getaddrinfo(NULL, port, &m_hints, &m_result);
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
			m_result = nullptr;
			closesocket(m_listenSocket);
			m_listenSocket = INVALID_SOCKET;
			WinsockInitializer::close();
			return false;
		}

		freeaddrinfo(m_result);
		m_result = nullptr;

		return true;
	}

	bool Server::listenSocket()
	{
		if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			closesocket(m_listenSocket);
			m_listenSocket = INVALID_SOCKET;
			WinsockInitializer::close();
			return false;
		}

		return true;
	}

	void Server::acceptLoop()
	{
		u_long mode = 1;
		ioctlsocket(m_listenSocket, FIONBIO, &mode);

		SOCKET clientSocket;
		while (m_isRunning)
		{
			clientSocket = accept(m_listenSocket, NULL, NULL);
			if (clientSocket == INVALID_SOCKET)
			{
				int err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK)
				{
					ServiceLocator::logger().logWarning(static_cast<std::string>("Accept failed: ") + std::to_string(WSAGetLastError()) + "\n");
				}
				
				continue;
			}

			ServiceLocator::logger().logInfo(static_cast<std::string>("New client connected! Socket: ") + std::to_string(clientSocket) + "\n");
			m_clients.push_back(clientSocket);
		}
	}
}
