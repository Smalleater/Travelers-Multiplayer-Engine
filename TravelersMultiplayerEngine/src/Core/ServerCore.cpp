#include "Core/ServerCore.hpp"

#include "ServiceLocator.hpp"
#include "Utils.hpp"
#include "TcpMessage.hpp"

namespace tme
{
    ServerCore::~ServerCore()
    {
        Stop();
    }

    ErrorCodes ServerCore::Start(uint16_t port)
    {
        if (IsStarted())
        {
            ServiceLocator::Logger().LogWarning("ServerCore::Start: Called but server is already started");
            return ErrorCodes::ServerAlreadyStarted;
        }

        ErrorCodes ecResult;
        int lastSocketError;

        m_tcpSocket = std::make_unique<TcpSocket>();

        ecResult = m_tcpSocket->Bind(port);
        lastSocketError = m_tcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("ServerCore::Start: Bind", ecResult, lastSocketError);
            return ecResult;
        }

        ecResult = m_tcpSocket->Listen();
        lastSocketError = m_tcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("ServerCore::Start: Listen", ecResult, lastSocketError);
            return ecResult;
        }

        ecResult = m_tcpSocket->SetBlocking(false);
        lastSocketError = m_tcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("ServerCore::Start: SetBlocking", ecResult, lastSocketError);
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes ServerCore::Stop()
    {
        if (!IsStarted())
        {
            return ErrorCodes::Success;
        }

        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;
        int lastSocketError;

        ecResult = DisconnectAllClient();
        if (ecResult != ErrorCodes::Success)
        {
            hadError = true;
        }
        else
        {
            hadSuccess = true;
        }

        ecResult = m_tcpSocket->Shutdown();
        lastSocketError = m_tcpSocket->GetLastSocketError();
        m_tcpSocket.reset();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("ServerCore::Stop: Shutdown server socket", ecResult, lastSocketError);
            hadError = true;
        }
        else
        {
            hadSuccess = true;
        }

        m_receivedTcpThisTick.clear();
        m_tcpPerClientSendQueue.clear();
        m_tcpBroadcastSendQueue.clear();

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes ServerCore::BeginUpdate()
    {
        if (!IsStarted())
        {
            ServiceLocator::Logger().LogWarning("ServerCore::BeginUpdate: Called but server are not started");
            return ErrorCodes::ServerNotStarted;
        }

        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        m_disconnectedClientsLastTick = std::move(m_disconnectedClientsThisTick);

        ecResult = Accept();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        ecResult = ReceivedTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes ServerCore::EndUpdate()
    {
        if (!IsStarted())
        {
            ServiceLocator::Logger().LogWarning("ServerCore::EndUpdate called but server are not started");
            return ErrorCodes::ServerNotStarted;
        }

        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        ecResult = SendToTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        ecResult = SendToAllTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    bool ServerCore::IsStarted()
    {
        return m_tcpSocket != nullptr;
    }

    const std::vector<uint32_t>& ServerCore::GetNewClientsThisTick() const
    {
        return m_newClientsThisTick;
    }

    const std::vector<uint32_t>& ServerCore::GetDisconnectedClientLastTick() const
    {
        return m_disconnectedClientsThisTick;
    }

    const std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& ServerCore::GetReceivedTcpThisTick() const
    {
        return m_receivedTcpThisTick;
    }

    void ServerCore::AddMessageToTcpPerClientSendQueue(uint32_t networkId, const std::vector<uint8_t>& data)
    {
        m_tcpPerClientSendQueue.emplace_back(networkId, data);
    }

    void ServerCore::AddMessageToTcpBroadcastSendQueue(const std::vector<uint8_t>& data)
    {
        m_tcpBroadcastSendQueue.push_back(data);
    }

    ErrorCodes ServerCore::DisconnectClient(uint32_t networkId)
    {
        if (m_clients.find(networkId) == m_clients.end())
        {
            ServiceLocator::Logger().LogError("ServerCore::DisconnectClient: No client found for networkId: " 
                + std::to_string(networkId));
            return ErrorCodes::ClientNotFound;
        }

        ErrorCodes ecResult = m_clients[networkId]->Shutdown();
        int lastSocketError = m_clients[networkId]->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("ServerCore::DisconnectClient: " 
                + std::string("Failed to shut down the socket associated with network ID: ") 
                + std::to_string(networkId) + " Socket error: " + std::to_string(lastSocketError));
        }

        m_clients.erase(networkId);
        m_clientMessageIdGenerators.erase(networkId);
        m_receiveBuffer.erase(networkId);

        m_disconnectedClientsThisTick.push_back(networkId);

        ServiceLocator::Logger().LogInfo("Client with id: " + std::to_string(networkId) + " disconnected");

        return ErrorCodes::Success;
    }

    ErrorCodes ServerCore::DisconnectAllClient()
    {
        std::vector<uint32_t> networkIds;

        for (std::pair<const uint32_t, std::unique_ptr<TcpSocket>>& clientPair : m_clients)
        {
            networkIds.push_back(clientPair.first);
        }

        for (uint32_t networkId : networkIds)
        {
            DisconnectClient(networkId);
        }

        return ErrorCodes::Success;
    }

    ErrorCodes ServerCore::Accept()
    {
        m_newClientsThisTick.clear();

        bool hadSuccess = false;
        bool hadError = false;

        const uint8_t maxAcceptPerFrame = 16;
        uint8_t acceptedThisFrame = 0;

        std::unique_ptr<ISocket> outClient;
        std::unique_ptr<TcpSocket> tcpClient;

        ErrorCodes ecResult;
        int lastSocketError;

        uint32_t networkId;

        while (acceptedThisFrame < maxAcceptPerFrame)
        {
            ecResult = m_tcpSocket->Accept(outClient);
            lastSocketError = m_tcpSocket->GetLastSocketError();

            if (ecResult != ErrorCodes::Success)
            {
                if (ecResult != ErrorCodes::AcceptWouldBlock)
                {
                    Utils::LogSocketError("ServerCore::Accept: Accept new client", ecResult, lastSocketError);
                    hadError = true;
                }

                break;
            }

            if (outClient == nullptr)
            {
                continue;
            }

            tcpClient = std::unique_ptr<TcpSocket>(dynamic_cast<TcpSocket*>(outClient.release()));
            if (tcpClient == nullptr)
            {
                ServiceLocator::Logger().LogError("ServerCore::Accept: Failed to cast ISocket* to TcpSocket*");
                hadError = true;
                continue;
            }

            tcpClient->SetBlocking(false);
            
            networkId = m_nextNetworkId++;
            m_clients[networkId] = std::move(tcpClient);
            m_clientMessageIdGenerators[networkId] = std::make_unique<MessageIdGenerator>();
            m_newClientsThisTick.push_back(networkId);
            
            ServiceLocator::Logger().LogInfo("New client connected, with network id: " 
                + std::to_string(networkId));

            hadSuccess = true;

            acceptedThisFrame++;
        }

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes ServerCore::ReceivedTcp()
    {
        m_receivedTcpThisTick.clear();

        if (m_clients.empty())
        {
            return ErrorCodes::Success;
        }

        bool hadSuccess = false;
        bool hadError = false;

        const uint8_t maxMessagesPerClientPerFrame = 32;
        uint8_t messagesReceivedThisFrame;

        int bytesReceived;
        std::vector<uint8_t> tempBuffer(4096);

        ErrorCodes ecResult;
        int lastSocketError;

        std::vector<uint32_t> clientsToRemove;

        for(std::pair<const uint32_t, std::unique_ptr<TcpSocket>>& clientPair : m_clients)
        {
            if (clientPair.second == nullptr)
            {
                continue;
            }

            uint32_t networkId = clientPair.first;
            std::unique_ptr<TcpSocket>& clientSocket = clientPair.second;

            messagesReceivedThisFrame = 0;
            while (messagesReceivedThisFrame < maxMessagesPerClientPerFrame)
            {
                bytesReceived = 0;
                tempBuffer.resize(4096);

                ecResult = clientSocket->Receive(tempBuffer.data(), tempBuffer.size(), bytesReceived);
                lastSocketError = clientSocket->GetLastSocketError();
                if (ecResult != ErrorCodes::Success)
                {
                    if (ecResult == ErrorCodes::ReceiveConnectionClosed)
                    {
                        clientsToRemove.push_back(networkId);
                        ServiceLocator::Logger().LogInfo("Disconnected from client (connection closed by remote host)");
                    }
                    else if (ecResult != ErrorCodes::ReceiveWouldBlock)
                    {
                        Utils::LogSocketError("ServerCore::ReceiveAllFromTcp: Receive", ecResult, lastSocketError);
                        clientsToRemove.push_back(networkId);
                        hadError = true;
                    }

                    break;
                }
                else
                {
                    hadSuccess = true;
                }

                tempBuffer.resize(bytesReceived);
                m_receiveBuffer[networkId].insert(m_receiveBuffer[networkId].end(), tempBuffer.begin(), tempBuffer.end());

                while (TcpMessage::HasCompleteHeader(m_receiveBuffer[networkId]) && messagesReceivedThisFrame < maxMessagesPerClientPerFrame)
                {
                    uint32_t payloadSize = TcpMessage::GetPayloadSizeFromHeader(m_receiveBuffer[networkId]);
                    uint32_t messageSize = TcpMessage::HEADER_SIZE + payloadSize;

                    if (!TcpMessage::HasCompleteMessage(m_receiveBuffer[networkId]))
                    {
                        break;
                    }

                    TcpMessage message(MessageType::UNKNOWN);
                    if (message.Deserialize(m_receiveBuffer[networkId].data(), messageSize))
                    {
                        std::vector<uint8_t> payload = message.GetPayload();
                        m_receivedTcpThisTick.emplace_back(networkId, std::move(payload));

                        messagesReceivedThisFrame++;
                    }
                    else
                    {
                        ServiceLocator::Logger().LogError("ServerCore::ReceivedTcp: Failed to deserialize TCP message");
                        m_receiveBuffer[networkId].clear();
                        break;
                    }

                    m_receiveBuffer[networkId].erase(m_receiveBuffer[networkId].begin(), m_receiveBuffer[networkId].begin() + messageSize);
                }

                if (m_receiveBuffer[networkId].size() > 64 * 1024)
                {
                    ServiceLocator::Logger().LogWarning("ServerCore::ReceivedTcp: Receive buffer to large, clearing");
                    m_receiveBuffer[networkId].clear();
                    break;
                }
            }
        }

        for (uint32_t networkId : clientsToRemove)
        {
            DisconnectClient(networkId);
        }

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes ServerCore::SendToTcp()
    {
        if (m_clients.empty() || m_tcpPerClientSendQueue.empty())
        {
            m_tcpPerClientSendQueue.clear();
            return ErrorCodes::Success;
        }

        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        for (const std::pair<uint32_t, std::vector<uint8_t>>& messagePair : m_tcpPerClientSendQueue)
        {
            if (m_clients.find(messagePair.first) == m_clients.end())
            {
                ServiceLocator::Logger().LogError("ServerCore::SendToClientTcp: No TCP client found for networkId: " 
                    + std::to_string(messagePair.first));
                hadError = true;
                continue;
            }

            ecResult = SendTcp(messagePair.first, messagePair.second);
            if (ecResult != ErrorCodes::Success && ecResult != ErrorCodes::SendConnectionClosed)
            {
                hadError = true;
                continue;
            }

            hadSuccess = true;
        }

        m_tcpPerClientSendQueue.clear();

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes ServerCore::SendToAllTcp()
    {
        if (m_clients.empty() || m_tcpBroadcastSendQueue.empty())
        {
            m_tcpBroadcastSendQueue.clear();
            return ErrorCodes::Success;
        }

        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        for (std::pair<const uint32_t, std::unique_ptr<TcpSocket>>& clientPair : m_clients)
        {
            for (const std::vector<uint8_t>& message : m_tcpBroadcastSendQueue)
            {
                ecResult = SendTcp(clientPair.first, message);
                if (ecResult == ErrorCodes::Success)
                {
                    hadSuccess = true;
                }
                else if (ecResult != ErrorCodes::SendConnectionClosed)
                {
                    hadError = true;
                    break;
                }
                else
                {
                    break;
                }
            }
        }

        m_tcpBroadcastSendQueue.clear();

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes ServerCore::SendTcp(uint32_t networkId, const std::vector<uint8_t>& data)
    {
        ErrorCodes ecResult;
        int lastSocketError;

        int bytesSent;

        TcpMessage message(MessageType::GAME_DATA, m_clientMessageIdGenerators[networkId]->GetUniqueId());
        message.SetPayload(data);

        std::vector<uint8_t> serializedMessage = message.Serialize();

        ecResult = m_clients[networkId]->Send(serializedMessage.data(), serializedMessage.size(), bytesSent);
        lastSocketError = m_clients[networkId]->GetLastSocketError();
        if (ecResult == ErrorCodes::SendConnectionClosed)
        {
            DisconnectClient(networkId);
            return ErrorCodes::SendConnectionClosed;
        }
        else if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("ServerCore::SendTcp: Send", ecResult, lastSocketError);
            DisconnectClient(networkId);
            return ecResult;
        }

        return ErrorCodes::Success;
    }
}