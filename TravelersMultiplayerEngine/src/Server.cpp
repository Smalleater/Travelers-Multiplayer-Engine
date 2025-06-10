#include "Server.h"

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
	bool Server::start(const char* port)
	{
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
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		int iResult = getaddrinfo(NULL, port, &hints, &result);
		if (iResult != 0)
		{
			WinsockInitializer::cleanup();
			return false;
		}

		SOCKET listenSocket = INVALID_SOCKET;

		listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (listenSocket == INVALID_SOCKET)
		{
			freeaddrinfo(result);
			WinsockInitializer::cleanup();
			return false;
		}

		return true;
	}
}
