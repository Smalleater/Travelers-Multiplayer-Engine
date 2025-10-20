#ifndef TME_CORE_UDP_SOCKET_HPP
#define TME_CORE_UPD_SOCKET_HPP

#include <utility>
#include <mutex>

#include "TME/errorCode.hpp"
#include "core/networkInclude.hpp"

namespace tme::core
{
    class UdpSocket
    {
    public:
        UdpSocket();
        ~UdpSocket();

        static sockaddr* createSockAddr(const char* address, uint16_t port);

        void closeSocket();
        std::pair<ErrorCode, int> bindSocket(const uint16_t port);
        std::pair<ErrorCode, int> sendDataTo(const void* data, size_t size, const sockaddr* destAddr);
        std::pair<ErrorCode, int> receiveDataFrom(void* buffer, size_t size, sockaddr* srcAddr);
        std::pair<ErrorCode, int> setBlocking(bool blocking);
        bool isOpen() const;

    private:
        socket_t m_socket;
        mutable std::mutex m_mutex;
    };
}

#endif