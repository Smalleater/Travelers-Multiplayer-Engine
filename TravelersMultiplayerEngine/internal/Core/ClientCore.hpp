#ifndef TME_CLIENT_CORE_HPP
#define TME_CLIENT_CORE_HPP

#include <vector>

#include "TME/ErrorCodes.hpp"

#include "sockets/TcpSocket.hpp"
#include "MessageIdGenerator.hpp"

namespace tme
{
    class ClientCore
    {
    private:
        std::unique_ptr<TcpSocket> m_tcpSocket;
        std::unique_ptr<MessageIdGenerator> m_messageIdGenerator;

        std::vector<uint8_t> m_receiveBuffer;
        std::vector<std::vector<uint8_t>> m_receivedTcpThisTick;

        std::vector<std::vector<uint8_t>> m_tcpSendQueue;

        ErrorCodes ReceivedTcp();

        ErrorCodes SendTcp();

    public:
        ClientCore() = default;
        ~ClientCore();

        ClientCore(const ClientCore&) = delete;
        ClientCore& operator=(const ClientCore&) = delete;

        ErrorCodes ConnectTo(const std::string& address, uint16_t port);
        ErrorCodes Disconnect();
        ErrorCodes BeginUpdate();
        ErrorCodes EndUpdate();

        bool IsConnected();

        const std::vector<std::vector<uint8_t>>& GetReceivedTcpThisTick() const;

        void AddMessageToTcpSendQueue(const std::vector<uint8_t>& data);
    };
}

#endif