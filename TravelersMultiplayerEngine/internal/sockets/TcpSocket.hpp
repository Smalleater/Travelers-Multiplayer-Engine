#ifndef TME_TCP_SOCKET_HPP
#define TME_TCP_SOCKET_HPP

#include "TME/ErrorCodes.hpp"

#include "interfaces/ISocket.hpp"

namespace tme
{
    class TcpSocket : public ISocket
    {
    private:

    public:
        TcpSocket() {};
        ~TcpSocket() {};

        ErrorCodes Init() override;
        ErrorCodes Shutdown() override;

        ErrorCodes Connect(const std::string& adress, uint16_t port) override;
        ErrorCodes Bind(uint16_t port) override;
        ErrorCodes Listen(int backlog = SOMAXCONN) override;
        
        std::unique_ptr<ISocket> Accept() override;

        ErrorCodes Send(const void* data, size_t size, int& bytesSent) override;
        ErrorCodes Receive(void* buffer, size_t size, int& bytesReceived) override;

        ErrorCodes SetBlocking(bool blocking) override;
        bool IsConnected() const override;
        socket_t GetNativeSocket() const override;
    };
}

#endif