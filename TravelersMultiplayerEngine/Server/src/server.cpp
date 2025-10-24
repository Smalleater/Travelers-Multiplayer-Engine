#include "TME/server/server.hpp"

namespace tme::server
{
    Server* Server::m_singleton = nullptr;

    Server::Server()
    {
        m_tcpSocket = nullptr;
        m_udpSocket = nullptr;

        m_isStarted = false;
    }

    ErrorCode Server::Start(uint16_t _port)
    {
        if (m_isStarted)
        {
            return ErrorCode::ServerAlreadyStarted;
        }

        m_tcpSocket = new core::TcpSocket;

        std::pair<ErrorCode, int> pairResult;
       
        pairResult = m_tcpSocket->bindSocket(_port);
        if (pairResult.first != ErrorCode::Success)
        {

        }

        return ErrorCode::Success;
    }
}
