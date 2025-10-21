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

        std::pair<ErrorCode, int> shutdownSocket();
        void closeSocket();
        std::pair<ErrorCode, int> connectTo(const char* _adress, const uint16_t _port);
        std::pair<ErrorCode, int> bindSocket(const uint16_t _port);
        std::pair<ErrorCode, int> listenSocket(int _backlog = SOMAXCONN);
        std::pair<ErrorCode, int> acceptSocket(std::unique_ptr<TcpSocket>& _outClient);
        std::pair<ErrorCode, int> sendData(const void* _data, size_t _size);
        std::pair<ErrorCode, int> receiveData(void* _buffer, size_t _size);
        std::pair<ErrorCode, int> setBlocking(bool _blocking);
        bool isConnected() const;

    private:
        socket_t m_socket;
        mutable std::mutex m_mutex;
    };
}


#endif