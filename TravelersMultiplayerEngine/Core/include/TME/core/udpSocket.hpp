#ifndef TME_CORE_UDP_SOCKET_HPP
#define TME_CORE_UPD_SOCKET_HPP

#include <utility>
#include <mutex>

#include "TME/errorCode.hpp"
#include "networkInclude.hpp"

namespace tme::core
{
    class UdpSocket
    {
    public:
        UdpSocket();
        ~UdpSocket();

        static sockaddr* createSockAddr(const char* _address, uint16_t _port);

        void closeSocket();
        std::pair<ErrorCode, int> bindSocket(const uint16_t _port);
        std::pair<ErrorCode, int> sendDataTo(const void* _data, size_t _size, const sockaddr* _destAddr);
        std::pair<ErrorCode, int> receiveDataFrom(void* _buffer, size_t _size, sockaddr* _srcAddr);
        std::pair<ErrorCode, int> setBlocking(bool _blocking);
        bool isOpen() const;

    private:
        socket_t m_socket;
        mutable std::mutex m_mutex;
    };
}

#endif