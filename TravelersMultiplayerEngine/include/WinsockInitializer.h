#pragma once

#ifndef  WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>

namespace tme
{
	class WinsockInitializer
	{
	public:
		static bool initialize();
		static void cleanup();
		static bool isInitialized();

		static const WSADATA& getWSAData();

	private:
		static bool m_initialized;
		static WSADATA m_wsaData;
	};
}
