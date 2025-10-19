#ifndef TME_CORE_TCP_SOCKET_HPP
#define TME_CORE_TCP_SOCKET_HPP

#include <utility>
#include <memory>
#include <cstdint>
#include <mutex>

#include "TME/errorCode.hpp"
#include "core/networkInclude.hpp"

namespace tme::core
{
    class TcpSocket
    {
    public:
        TcpSocket();
        ~TcpSocket();

        std::pair<ErrorCode, int> Shutdown();
        std::pair<ErrorCode, int> Connect(const char* adress, const uint16_t);
        std::pair<ErrorCode, int> Bind(const uint16_t port);
        std::pair<ErrorCode, int> Listen(int backlog = SOMAXCONN);
        std::pair<ErrorCode, int> Accept(std::unique_ptr<TcpSocket>& outClient);
        std::pair<ErrorCode, int> Send(const void* data, size_t size);
        std::pair<ErrorCode, int> Receive(void* buffer, size_t size);
        std::pair<ErrorCode, int> SetBlocking(bool blocking);
        bool IsConnected() const;

    private:
        socket_t m_socket = INVALID_SOCKET_FD;
        mutable std::mutex m_mutex;
    };
}


#endif