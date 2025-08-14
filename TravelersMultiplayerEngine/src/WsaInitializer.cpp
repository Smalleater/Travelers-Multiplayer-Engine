#include "WsaInitializer.hpp"

#ifdef _WIN32

namespace tme
{
    // Initializes Winsock (version 2.2)
    ErrorCodes WsaInitializer::Init()
    {
        m_wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
        if (m_wsaStartupResult != 0)
        {
            return ErrorCodes::Failure;
        }

        return ErrorCodes::Success;
    }

    // Cleans up Winsock resources
    WsaInitializer::~WsaInitializer()
    {
        WSACleanup();
    }

    int WsaInitializer::GetWsaStartupResult() const
    {
        return m_wsaStartupResult;
    }
}

#endif