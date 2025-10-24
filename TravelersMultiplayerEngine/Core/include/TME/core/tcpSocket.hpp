#ifndef TME_CORE_TCP_SOCKET_HPP
#define TME_CORE_TCP_SOCKET_HPP

#include "TME/export.hpp"

#include <utility>
#include <memory>
#include <cstdint>
#include <mutex>
#include <string>

#include "TME/errorCode.hpp"
#include "networkInclude.hpp"

namespace tme::Core
{
    class TcpSocket
    {
    public:
        TME_API TcpSocket();
        TME_API ~TcpSocket();

        TME_API std::pair<ErrorCode, int> shutdownSocket();
        TME_API void closeSocket();
        TME_API std::pair<ErrorCode, int> connectTo(const std::string& _adress, const uint16_t _port);
        TME_API std::pair<ErrorCode, int> bindSocket(const uint16_t _port);
        TME_API std::pair<ErrorCode, int> listenSocket(int _backlog = SOMAXCONN);
        TME_API std::pair<ErrorCode, int> acceptSocket(TcpSocket** _outClient);
        TME_API std::pair<ErrorCode, int> sendData(const void* _data, size_t _size);
        TME_API std::pair<ErrorCode, int> receiveData(void* _buffer, size_t _size);
        TME_API std::pair<ErrorCode, int> setBlocking(bool _blocking);
        TME_API bool isConnected() const;

    private:
        socket_t m_socket;
        mutable std::mutex m_mutex;
    };
}


#endif