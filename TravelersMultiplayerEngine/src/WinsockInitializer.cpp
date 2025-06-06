#include "WinsockInitializer.h"

namespace tme
{
	bool WinsockInitializer::m_initialized = false;
	WSADATA WinsockInitializer::m_wsaData;


	bool WinsockInitializer::initialize()
	{
		if (!m_initialized)
		{
			int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
			if (result != 0)
			{
				return false;
			}

			m_initialized = true;
		}

		return true;
	}

	void WinsockInitializer::cleanup()
	{
		if (m_initialized)
		{
			WSACleanup();
			m_initialized = false;
		}
	}

	bool WinsockInitializer::isInitialized()
	{
		return m_initialized;
	}

	const WSADATA& WinsockInitializer::getWSAData()
	{
		return m_wsaData;
	}
}