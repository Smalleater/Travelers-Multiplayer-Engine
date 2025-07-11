#include "TME/Old/WinsockInitializer.h"

namespace tme
{
	bool WinsockInitializer::m_started = false;
	WSADATA WinsockInitializer::m_wsaData;


	bool WinsockInitializer::start()
	{
		if (!m_started)
		{
			int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
			if (result != 0)
			{
				return false;
			}

			m_started = true;
		}

		return true;
	}

	void WinsockInitializer::close()
	{
		if (m_started)
		{
			WSACleanup();
			m_started = false;
		}
	}

	bool WinsockInitializer::isStarted()
	{
		return m_started;
	}
}