#include "ServerCore.hpp"

#include "ServiceLocator.hpp"
#include "Utils.hpp"

namespace tme
{
    ErrorCodes ServerCore::Start(uint16_t port)
    {
        if (m_tcpSocket != nullptr)
        {
            ServiceLocator::Logger().LogWarning("Server::Start called but server socket already exists");
            return ErrorCodes::AlreadyStarted;
        }

        ErrorCodes ecResult;
        int lastSocketError;

        m_tcpSocket = std::make_unique<TcpSocket>();

        ecResult = m_tcpSocket->Bind(port);
        lastSocketError = m_tcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("Server::Start: Bind", ecResult, lastSocketError);
            return ecResult;
        }

        ecResult = m_tcpSocket->Listen();
        lastSocketError = m_tcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("Server::Start: Listen", ecResult, lastSocketError);
            return ecResult;
        }

        ecResult = m_tcpSocket->SetBlocking(false);
        lastSocketError = m_tcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("Server::Start: SetBlocking", ecResult, lastSocketError);
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes ServerCore::BeginUpdate()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        ecResult = Accept();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        ecResult = ReceivedTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes ServerCore::EndUpdate()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        ecResult = SendToClientTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        ecResult = SendToAllClientTcp();
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
                    Utils::LogSocketError("ServerAcceptClient: Accept new client", ecResult, lastSocketError);
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
                ServiceLocator::Logger().LogError("UpdateServer: Failed to cast ISocket* to TcpSocket*");
                hadError = true;
                continue;
            }

            tcpClient->SetBlocking(false);
            
            networkId = m_nextNetworkId++;
            m_clients[networkId] = std::move(tcpClient);
            ServiceLocator::Logger().LogInfo("New client connected, with network id: " 
                + std::to_string(networkId));

            m_newClientsThisTick.push_back(networkId);

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
        std::vector<uint8_t> buffer(4096);

        ErrorCodes ecResult;
        int lastSocketError;

        std::vector<uint32_t> clientsToRemove;

        for(std::pair<const uint32_t, std::unique_ptr<TcpSocket>>& clientPair : m_clients)
        {
            uint32_t networkId = clientPair.first;
            std::unique_ptr<TcpSocket>& clientSocket = clientPair.second;

            messagesReceivedThisFrame = 0;
            while (messagesReceivedThisFrame < maxMessagesPerClientPerFrame)
            {
                bytesReceived = 0;
                buffer.resize(4096);

                ecResult = clientSocket->Receive(buffer.data(), buffer.size(), bytesReceived);
                lastSocketError = clientSocket->GetLastSocketError();
                if (ecResult != ErrorCodes::Success)
                {
                    if (ecResult == ErrorCodes::ReceiveConnectionClosed)
                    {
                        clientsToRemove.push_back(networkId);
                    }
                    else if (ecResult != ErrorCodes::ReceiveWouldBlock)
                    {
                        Utils::LogSocketError("ReceiveAllFromServerTcp: Receive", ecResult, lastSocketError);
                        clientsToRemove.push_back(networkId);
                        hadError = true;
                    }

                    break;
                }
                else
                {
                    hadSuccess = true;
                }

                buffer.resize(bytesReceived);
                m_receivedTcpThisTick.emplace_back(networkId, std::move(buffer));

                messagesReceivedThisFrame++;
            }
        }

        for (uint32_t networkId : clientsToRemove)
        {
            m_clients[networkId]->Shutdown();
            m_clients.erase(networkId);
            ServiceLocator::Logger().LogInfo("Client with id: " + std::to_string(networkId) 
                + " disconnected");
        }

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes ServerCore::SendToClientTcp()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        for (const std::pair<uint32_t, std::vector<uint8_t>>& messagePair : m_tcpPerClientSendQueue)
        {
            if (m_clients.find(messagePair.first) == m_clients.end())
            {
                ServiceLocator::Logger().LogError("ServerSendToClientTcp: No TCP client found for networkId" 
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

    ErrorCodes ServerCore::SendToAllClientTcp()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        for (std::pair<const uint32_t, std::unique_ptr<TcpSocket>>& clientPair : m_clients)
        {
            for (const std::vector<uint8_t>& message : m_tcpBroadcastSendQueue)
            {
                ecResult = SendTcp(clientPair.first, message);
                if (ecResult != ErrorCodes::Success)
                {
                    if (ecResult != ErrorCodes::SendConnectionClosed)
                    {
                        hadError = true;
                    }

                    break;
                }

                hadSuccess = true;
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

        ecResult = m_clients[networkId]->Send(data.data(), data.size(), bytesSent);
        lastSocketError = m_clients[networkId]->GetLastSocketError();
        if (ecResult == ErrorCodes::SendConnectionClosed)
        {
            m_clients[networkId]->Shutdown();
            m_clients.erase(networkId);
            ServiceLocator::Logger().LogInfo("Client with id: " + std::to_string(networkId) 
                + " disconnected");
            return ErrorCodes::SendConnectionClosed;
        }
        else if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("ServerSendTcp: Send", ecResult, lastSocketError);

            m_clients[networkId]->Shutdown();
            m_clients.erase(networkId);
            ServiceLocator::Logger().LogInfo("Client with id: " + std::to_string(networkId) 
                + " disconnected");

            return ecResult;
        }

        return ErrorCodes::Success;
    }
}