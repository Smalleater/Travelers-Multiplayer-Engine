#include "TME/Old/Client.h"

#include "TME/Old/Utils.h"
#include "TME/Old/WinsockInitializer.h"

namespace tme
{
	addrinfo* Client::m_result = nullptr;
	addrinfo Client::m_hints;
	SOCKET Client::m_connectSocket = INVALID_SOCKET;
	bool Client::m_isRunning = false;

	bool Client::connectTo(const char* ip, const char* port)
	{
		if (m_isRunning)
		{
			return false;
		}

		if (!Utils::isValidIPv4(ip))
		{
			return false;
		}

		if (!Utils::isValidPort(port))
		{
			return false;
		}

		if (WinsockInitializer::isStarted())
		{
			return false;
		}

		if (!WinsockInitializer::start())
		{
			return false;
		}

		if (!createSocket(ip, port))
		{
			return false;
		}

		if (!connectSocket())
		{
			return false;
		}

		return true;
	}

	bool Client::isRunning()
	{
		return m_isRunning;
	}

	bool Client::createSocket(const char* ip, const char* port)
	{
		ZeroMemory(&m_hints, sizeof(m_hints));
		m_hints.ai_family = AF_UNSPEC;
		m_hints.ai_socktype = SOCK_STREAM;
		m_hints.ai_protocol = IPPROTO_TCP;

		int result;

		result = getaddrinfo(ip, port, &m_hints, &m_result);
		if (result != 0)
		{
			WinsockInitializer::close();
			return false;
		}

		return true;
	}

	bool Client::connectSocket()
	{
		int result;

		addrinfo* ptr = m_result;
		while (ptr != nullptr)
		{
			m_connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (m_connectSocket == INVALID_SOCKET)
			{
				ptr = ptr->ai_next;
				continue;
			}

			result = connect(m_connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (result == SOCKET_ERROR)
			{
				closesocket(m_connectSocket);
				ptr = ptr->ai_next;
				continue;
			}

			break;
		}

		freeaddrinfo(m_result);
		m_result = nullptr;

		if (m_connectSocket == INVALID_SOCKET)
		{
			WinsockInitializer::close();
			return false;
		}

		return true;
	}
}
