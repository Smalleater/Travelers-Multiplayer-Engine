#pragma once

#include <WinSock2.h>

namespace tme
{
	class WinsockInitializer
	{
	public:
		static bool start();
		static void close();
		static bool isStarted();

		static const WSADATA& getWSAData();

	private:
		static bool m_started;
		static WSADATA m_wsaData;
	};
}
