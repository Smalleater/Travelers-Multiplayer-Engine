#include "TME/server/server.hpp"

#include "TME/core/tcpSocket.hpp"
#include "TME/core/udpSocket.hpp"

namespace tme::Server
{
    Core::TcpSocket* m_tcpSocket;
    Core::UdpSocket* m_udpSocket;

    void Start(uint16_t _port)
    {

    }
}
