#ifndef TME_NETWORK_MANAGER_HPP
#define TME_NETWORK_MANAGER_HPP

#include <memory>

#include "TME/ErrorCodes.hpp"

#include "sockets/TcpSocket.hpp"

namespace tme
{
    class NetworkManager
    {
    private:
        std::unique_ptr<TcpSocket> m_serverTcpSocket;

        std::unique_ptr<TcpSocket> m_clientTcpSocket;

        std::unordered_map<uint32_t, std::unique_ptr<TcpSocket>> m_clients;
        uint32_t m_nextClientId = 0;

        ErrorCodes UpdateServer();
        ErrorCodes UpdateClient();

    public:
        NetworkManager() {}
        ~NetworkManager() {}

        ErrorCodes StartServer(uint16_t port);
        ErrorCodes StartClient(const std::string& address, uint16_t port);

        ErrorCodes Update();
    };    
}

#endif