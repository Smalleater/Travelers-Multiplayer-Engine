#ifndef TME_SERVER_CORE_HPP
#define TME_SERVER_CORE_HPP

#include <unordered_map>
#include <vector>

#include "TME/ErrorCodes.hpp"

#include "sockets/TcpSocket.hpp"
#include "MessageIdGenerator.hpp"

namespace tme
{
    class ServerCore
    {
    private:
        std::unique_ptr<TcpSocket> m_tcpSocket;

        std::unordered_map<uint32_t, std::unique_ptr<TcpSocket>> m_clients;
        std::unordered_map<uint32_t, std::unique_ptr<MessageIdGenerator>> m_clientMessageIdGenerators;

        uint32_t m_nextNetworkId = 0;

        std::vector<uint32_t> m_newClientsThisTick;

        std::vector<std::pair<uint32_t, std::vector<uint8_t>>> m_receivedTcpThisTick;

        std::vector<std::pair<uint32_t, std::vector<uint8_t>>> m_tcpPerClientSendQueue;
        std::vector<std::vector<uint8_t>> m_tcpBroadcastSendQueue;

        ErrorCodes Accept();
        ErrorCodes ReceivedTcp();

        ErrorCodes SendToTcp();
        ErrorCodes SendToAllTcp();
        ErrorCodes SendTcp(uint32_t networkId, const std::vector<uint8_t>& data);

    public:
        ServerCore() = default;
        ~ServerCore();

        ServerCore(const ServerCore&) = delete;
        ServerCore& operator=(const ServerCore&) = delete;

        ErrorCodes Start(uint16_t port);
        ErrorCodes Stop();
        ErrorCodes BeginUpdate();
        ErrorCodes EndUpdate();

        bool IsStarted();

        const std::vector<uint32_t>& GetNewClientsThisTick() const;
        const std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& GetReceivedTcpThisTick() const;

        void AddMessageToTcpPerClientSendQueue(uint32_t networkId, const std::vector<uint8_t>& data);
        void AddMessageToTcpBroadcastSendQueue(const std::vector<uint8_t>& data);

        ErrorCodes DisconnectClient(uint32_t networkId);
        ErrorCodes DisconnectAllClient();
    };
}

#endif