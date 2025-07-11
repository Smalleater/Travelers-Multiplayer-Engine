#pragma once

#include "../TMEExport.h"

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
	class TME_API Client
	{
	public:
		static bool connectTo(const char* ip, const char* port);
		//static bool disconnect();

		static bool isRunning();

	private:
		static addrinfo* m_result;
		static addrinfo m_hints;
		static SOCKET m_connectSocket;
		static bool m_isRunning;

		static bool createSocket(const char* ip, const char* port);
		static bool connectSocket();
	};
}
