#include "TME/core/tcpSocket.hpp"

#include <limits>
#include <cstdio>

#include "TME/debugUtils.hpp"
#include "socketUtils.hpp"

#undef max

namespace tme::core
{
	TcpSocket::TcpSocket()
	{
		m_socket = INVALID_SOCKET_FD;
	}

	TcpSocket::~TcpSocket()
	{
		shutdownSocket();
		closeSocket();
	}

	std::pair<ErrorCode, int> TcpSocket::shutdownSocket()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_socket == INVALID_SOCKET_FD)
		{
			return { ErrorCode::Success, 0 };
		}

		int iResult = shutdown(m_socket, SHUTDOWN_BOTH);
		int lastSocketError = SocketUtils::GetLastSocketError();
		if (iResult != 0 && lastSocketError != SOCKET_NOT_CONNECTED)
		{
			return { ErrorCode::SocketShutdownFailed, lastSocketError };
		}

		return { ErrorCode::Success, 0 };
	}

	void TcpSocket::closeSocket()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_socket != INVALID_SOCKET_FD)
		{
			CLOSE_SOCKET(m_socket);
			m_socket = INVALID_SOCKET_FD;
		}
	}

	std::pair<ErrorCode, int> TcpSocket::connectTo(const std::string& _address, uint16_t _port)
	{
		TME_ASSERT_REF_PTR_OR_COPIABLE(_address);

		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_socket != INVALID_SOCKET_FD)
		{
			return { ErrorCode::SocketAlreadyOpen, 0 };
		}

		addrinfo hints = {};
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		addrinfo* result = nullptr;

		int iResult = getaddrinfo(_address.c_str(), std::to_string(_port).c_str(), &hints, &result);
		int lastSocketError = SocketUtils::GetLastSocketError();
		if (iResult != 0 || result == nullptr)
		{
			return { ErrorCode::SocketGetaddrinfoFailed, lastSocketError };
		}

		for (addrinfo* rp = result; rp != nullptr; rp = rp->ai_next)
		{
			m_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (m_socket == INVALID_SOCKET_FD)
			{
				continue;
			}

			iResult = connect(m_socket, rp->ai_addr, static_cast<int>(rp->ai_addrlen));
			if (iResult == 0)
			{
				freeaddrinfo(result);
				return { ErrorCode::Success, 0 };
			}

			closeSocket();
		}

		freeaddrinfo(result);
		return { ErrorCode::SocketConnectFailed, 0 };
	}

	std::pair<ErrorCode, int> TcpSocket::bindSocket(uint16_t _port)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_socket != INVALID_SOCKET_FD)
		{
			return { ErrorCode::SocketAlreadyOpen, 0 };
		}

		addrinfo hints = {};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		addrinfo* result = nullptr;

		char portStr[6];
		snprintf(portStr, sizeof(portStr), "%u", _port);

		int iResult = getaddrinfo(NULL, portStr, &hints, &result);
		int lastSocketError = SocketUtils::GetLastSocketError();
		if (iResult != 0 || result == nullptr)
		{
			return { ErrorCode::SocketGetaddrinfoFailed, lastSocketError };
		}

		for (addrinfo* rp = result; rp != nullptr; rp = rp->ai_next)
		{
			m_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (m_socket == INVALID_SOCKET_FD)
			{
				continue;
			}

			iResult = bind(m_socket, rp->ai_addr, static_cast<int>(rp->ai_addrlen));
			if (iResult == 0)
			{
				freeaddrinfo(result);
				return { ErrorCode::Success, 0 };
			}

			closeSocket();
		}

		freeaddrinfo(result);
		return { ErrorCode::SocketBindFailed, 0 };
	}

	std::pair<ErrorCode, int> TcpSocket::listenSocket(int _backlog)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_socket == INVALID_SOCKET_FD)
		{
			return { ErrorCode::SocketNotOpen, 0 };
		}

		int iResult = listen(m_socket, _backlog);
		int lastSocketError = SocketUtils::GetLastSocketError();
		if (iResult < 0)
		{
			closeSocket();
			return { ErrorCode::SocketListenFailed, lastSocketError };
		}

		return { ErrorCode::Success, 0 };
	}

	std::pair<ErrorCode, int> TcpSocket::acceptSocket(TcpSocket** _outClient)
	{
		TME_ASSERT_REF_PTR_OR_COPIABLE(_outClient);

		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_socket == INVALID_SOCKET_FD)
		{
			return { ErrorCode::SocketNotOpen, 0 };
		}

		socket_t clientSocket = accept(m_socket, NULL, NULL);
		int lastSocketError = SocketUtils::GetLastSocketError();
		if (clientSocket == INVALID_SOCKET_FD)
		{
			if (SocketUtils::IsWouldBlockError(lastSocketError))
			{
				return { ErrorCode::SocketWouldBlock, 0 };
			}

			return { ErrorCode::SocketAcceptFailed, lastSocketError };
		}

		*_outClient = new TcpSocket;
		(*_outClient)->m_socket = clientSocket;

		return { ErrorCode::Success, 0 };
	}

	std::pair<ErrorCode, int> TcpSocket::sendData(const void* _data, size_t _size)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_socket == INVALID_SOCKET_FD)
		{
			return { ErrorCode::SocketNotOpen, 0 };
		}

		if (_size > static_cast<size_t>(std::numeric_limits<int>::max()))
		{
			return { ErrorCode::SocketSendSizeTooLarge, 0 };
		}

		int iResult = send(m_socket, static_cast<const char*>(_data), static_cast<int>(_size), 0);
		int lastSocketError = SocketUtils::GetLastSocketError();
		if (iResult == 0)
		{
			return { ErrorCode::SocketConnectionClosed, 0 };
		}
		else if (iResult < 0)
		{
			return { ErrorCode::SocketSendFailed, lastSocketError };
		}

		return { ErrorCode::Success, 0 };
	}

	std::pair<ErrorCode, int> TcpSocket::receiveData(void* _buffer, size_t _size)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_socket == INVALID_SOCKET_FD)
		{
			return { ErrorCode::SocketNotOpen, 0 };
		}

		if (_size > static_cast<size_t>(std::numeric_limits<int>::max()))
		{
			return { ErrorCode::SocketReceiveSizeTooLarge, 0 };
		}

		int iResult = recv(m_socket, static_cast<char*>(_buffer), static_cast<int>(_size), 0);
		int lastSocketError = SocketUtils::GetLastSocketError();
		if (iResult == 0)
		{
			return { ErrorCode::SocketConnectionClosed, lastSocketError };
		}
		else if (iResult < 0)
		{
			if (SocketUtils::IsWouldBlockError(lastSocketError))
			{
				return { ErrorCode::SocketWouldBlock, 0 };
			}
			else if (lastSocketError == SOCKET_CONNECTION_RESET)
			{
				return { ErrorCode::SocketConnectionClosed, 0 };
			}

			return { ErrorCode::SocketReceiveFailed, lastSocketError };
		}

		return { ErrorCode::Success, 0 };
	}

	std::pair<ErrorCode, int> TcpSocket::setBlocking(bool _blocking)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return SocketUtils::setBlocking(m_socket, _blocking);
	}

	bool TcpSocket::isConnected() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		sockaddr_storage addr;
		socklen_t addrLen = sizeof(addr);

		return m_socket != INVALID_SOCKET_FD && getpeername(m_socket, (sockaddr*)&addr, &addrLen) == 0;
	}
}