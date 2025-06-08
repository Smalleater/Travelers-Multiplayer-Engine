#pragma once

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
