#ifndef TME_CORE_TCP_SOCKET_HPP
#define TME_CORE_TCP_SOCKET_HPP

#include "core/iSocket.hpp"

namespace tme::core
{
    class TcpSocket : public ISocket
    {
    public:
        TcpSocket() = default;
        ~TcpSocket();

        ErrorCode Shutdown() override;
        ErrorCode Connect(const char* adress, uint16_t) override;
        ErrorCode Bind(uint16_t port) override;
        ErrorCode Listen(int backlog = SOMAXCONN) override;
        ErrorCode Accept(std::unique_ptr<ISocket>& outClient) override;
        ErrorCode Send(const void* data, size_t size) override;
        ErrorCode Receive(void* buffer, size_t size) override;
        ErrorCode SetBlocking(bool blocking) override;
        bool IsConnected() const override;
    };
}


#endif