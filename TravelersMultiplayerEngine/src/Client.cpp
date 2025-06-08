#include "Client.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#include "Utils.h"
#include "WinsockInitializer.h"

#pragma comment(lib, "Ws2_32.lib")

namespace tme
{
	bool Client::start(const char* ipAdress, const char* port)
	{
		if (!Utils::isValidIPv4(ipAdress))
		{
			return false;
		}

		if (!Utils::isValidPort(port))
		{
			return false;
		}

		if (WinsockInitializer::isInitialized())
		{
			return false;
		}

		if (!WinsockInitializer::initialize())
		{
			return false;
		}

		addrinfo* result = NULL;
		addrinfo* ptr = NULL;
		addrinfo hints;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		int iResult;

		iResult = getaddrinfo(ipAdress, port, &hints, &result);
		if (iResult != 0)
		{
			WinsockInitializer::cleanup();
			return false;
		}

		SOCKET connectSocket = INVALID_SOCKET;

		ptr = result;

		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (connectSocket == INVALID_SOCKET)
		{
			freeaddrinfo(result);
			WinsockInitializer::cleanup();
			return false;
		}

		return true;
	}
}
