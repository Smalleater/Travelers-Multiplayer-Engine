#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

namespace tme
{
	class Server
	{
	public:
		static bool start(const char* port);
		static bool stop();

		static bool isRunning();

	private:
		static addrinfo* m_result;
		static addrinfo m_hints;
		static SOCKET m_listenSocket;
		static bool m_isRunning;
		static std::vector<SOCKET> m_clients;

		static bool createSocket(const char* port);
		static bool bindSocket();
		static bool listenSocket();

		static void acceptLoop();
	};
}
