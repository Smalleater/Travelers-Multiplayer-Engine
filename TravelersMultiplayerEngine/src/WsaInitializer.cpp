#include "WsaInitializer.hpp"

namespace tme
{
    // Initializes Winsock (version 2.2)
    ErrorCodes WsaInitializer::Init()
    {
        int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
        if (result != 0)
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
}