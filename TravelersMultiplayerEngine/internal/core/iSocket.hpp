#ifndef TME_CORE_I_SOCKET_HPP
#define TME_CORE_I_SOCKET_HPP

#include <memory>
#include <utility>

#include "TME/errorCode.hpp"

#include "core/networkInclude.hpp"

namespace tme::core
{
    class ISocket
    {
    public:
        ~ISocket() = default;

        virtual std::pair<ErrorCode, int> Shutdown() = 0;
        virtual ErrorCode Connect(const char* address, uint16_t port) = 0;
        virtual ErrorCode Bind(uint16_t port) = 0;
        virtual ErrorCode Listen(int backlog = SOMAXCONN) = 0;
        virtual ErrorCode Accept(std::unique_ptr<ISocket>& outClient) = 0;
        virtual ErrorCode Send(const void* data, size_t size) = 0;
        virtual ErrorCode Receive(void* buffer, size_t size) = 0;
        virtual ErrorCode SetBlocking(bool blocking) = 0;
        virtual bool IsConnected() const = 0;
    };
}

#endif