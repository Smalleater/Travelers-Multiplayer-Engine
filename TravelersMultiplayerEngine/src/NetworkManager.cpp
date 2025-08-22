#include "NetworkManager.hpp"

#include "ServiceLocator.hpp"
#include "Utils.hpp"

namespace tme
{
    const std::vector<uint32_t>& NetworkManager::GetNewClientsThisTick() const
    {
        return m_newClientsThisTick;
    }

    const std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& NetworkManager::GetServerReceivedTcpThisTick() const
    {
        return m_serverReceivedTcpThisTick;
    }

    const std::vector<std::vector<uint8_t>>& NetworkManager::GetClientReceivedTcpThisTick() const
    {
        return m_clientReceivedTcpThisTick;
    }

    bool NetworkManager::HasServerSocket() const
    {
        return m_serverTcpSocket != nullptr;
    }

    bool NetworkManager::HasClientSocket() const
    {
        return m_clientTcpSocket != nullptr;
    }

    void NetworkManager::AddMessageToServerTcpPerClientSendQueue(uint32_t networkId, const std::vector<uint8_t>& data)
    {
        m_serverTcpPerClientSendQueue.emplace_back(networkId, data);
    }

    void NetworkManager::AddMessageToServerTcpBroadcastSendQueue(const std::vector<uint8_t>& data)
    {
        m_serverTcpBroadcastSendQueue.push_back(data);
    }

    void NetworkManager::AddMessageToClientTcpSendQueue(const std::vector<uint8_t>& data)
    {
        m_clientTcpSendQueue.push_back(data);
    }

    // Starts the server, binds and listens on the given port
    // Initializes WSA on Windows if needed
    ErrorCodes NetworkManager::StartServer(uint16_t port)
    {
        ErrorCodes ecResult;
        int lastSocketError;

        if (m_serverTcpSocket != nullptr)
        {
            ServiceLocator::Logger().LogWarning("StartServer called but server socket already exists");
            return ErrorCodes::AlreadyInitialized;
        }

        #ifdef _WIN32
            if (m_wsa == nullptr)
            {
                m_wsa = std::make_unique<WsaInitializer>();
                
                ecResult = m_wsa->Init();
                lastSocketError = m_wsa->GetWsaStartupResult();
                if (ecResult != ErrorCodes::Success)
                {
                    Utils::LogSocketError("StartServer : WSA initialization", ecResult, lastSocketError);
                    return ecResult;
                }
            }
        #endif

        m_serverTcpSocket = std::make_unique<TcpSocket>();

        ecResult = m_serverTcpSocket->Bind(port);
        lastSocketError = m_serverTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("StartServer: Bind", ecResult, lastSocketError);
            return ecResult;
        }

        ecResult = m_serverTcpSocket->Listen();
        lastSocketError = m_serverTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("StartServer: Listen", ecResult, lastSocketError);
            return ecResult;
        }

        ecResult = m_serverTcpSocket->SetBlocking(false);
        lastSocketError = m_serverTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("StartServer: SetBlocking", ecResult, lastSocketError);
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    // Starts the client and connects to the given address and port
    // Initializes WSA on Windows if needed
    ErrorCodes NetworkManager::StartClient(const std::string& address, uint16_t port)
    {
        ErrorCodes ecResult;
        int lastSocketError;

        if (m_serverTcpSocket != nullptr)
        {
            ServiceLocator::Logger().LogWarning("StartClient called but client socket already exists");
            return ErrorCodes::AlreadyInitialized;
        }

        #ifdef _WIN32
            if (m_wsa == nullptr)
            {
                m_wsa = std::make_unique<WsaInitializer>();

                ecResult = m_wsa->Init();
                lastSocketError = m_wsa->GetWsaStartupResult();
                if (ecResult != ErrorCodes::Success)
                {
                    Utils::LogSocketError("StartClient: WSA initialization", ecResult, lastSocketError);
                    return ecResult;
                }
            }
        #endif

        m_clientTcpSocket = std::make_unique<TcpSocket>();

        ecResult = m_clientTcpSocket->Connect(address, port);
        lastSocketError = m_clientTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("StartClient: Connect", ecResult, lastSocketError);
            return ecResult;
        }

        ecResult = m_clientTcpSocket->SetBlocking(false);
        lastSocketError = m_clientTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("StartClient: SetBlocking", ecResult, lastSocketError);
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::BeginUpdate()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        if (m_serverTcpSocket != nullptr)
        {
            ecResult = BeginUpdateServer();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        if (m_clientTcpSocket != nullptr)
        {
            ecResult = BeginUpdateClient();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::EndUpdate()
    {
        bool hadSuccess = false;
        bool hadError = false;
        
        ErrorCodes ecResult;

        if (m_serverTcpSocket != nullptr)
        {
            ecResult = EndUpdateServer();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        if (m_clientTcpSocket != nullptr)
        {
            ecResult = EndUpdateClient();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::BeginUpdateServer()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        ecResult = ServerAccept();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        ecResult = ServerReceivedTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::EndUpdateServer()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        ecResult = ServerSendToClientTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        ecResult = ServerSendToAllClientTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::ServerAccept()
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
            ecResult = m_serverTcpSocket->Accept(outClient);
            lastSocketError = m_serverTcpSocket->GetLastSocketError();

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

    ErrorCodes NetworkManager::ServerReceivedTcp()
    {
        m_serverReceivedTcpThisTick.clear();

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
                m_serverReceivedTcpThisTick.emplace_back(networkId, std::move(buffer));

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

    ErrorCodes NetworkManager::ServerSendToClientTcp()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        for (const std::pair<uint32_t, std::vector<uint8_t>>& messagePair : m_serverTcpPerClientSendQueue)
        {
            if (m_clients.find(messagePair.first) == m_clients.end())
            {
                ServiceLocator::Logger().LogError("ServerSendToClientTcp: No TCP client found for networkId" 
                    + std::to_string(messagePair.first));
                hadError = true;
                continue;
            }

            ecResult = ServerSendTcp(messagePair.first, messagePair.second);
            if (ecResult != ErrorCodes::Success && ecResult != ErrorCodes::SendConnectionClosed)
            {
                hadError = true;
                continue;
            }

            hadSuccess = true;
        }

        m_serverTcpPerClientSendQueue.clear();

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::ServerSendToAllClientTcp()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        for (std::pair<const uint32_t, std::unique_ptr<TcpSocket>>& clientPair : m_clients)
        {
            for (const std::vector<uint8_t>& message : m_serverTcpBroadcastSendQueue)
            {
                ecResult = ServerSendTcp(clientPair.first, message);
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

        m_serverTcpBroadcastSendQueue.clear();

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::ServerSendTcp(uint32_t networkId, const std::vector<uint8_t>& data)
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

    ErrorCodes NetworkManager::BeginUpdateClient()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        ecResult = ClientReceivedTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::EndUpdateClient()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        ecResult = ClientSendTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::ClientReceivedTcp()
    {
        m_clientReceivedTcpThisTick.clear();

        const uint8_t maxMessagesPerFrame = 32;
        uint8_t messagesReceivedThisFrame = 0;

        int bytesReceived;
        std::vector<uint8_t> buffer(4096);

        ErrorCodes ecResult;
        int lastSocketError;

        while (messagesReceivedThisFrame < maxMessagesPerFrame)
        {
            bytesReceived = 0;
            buffer.resize(4096);

            ecResult = m_clientTcpSocket->Receive(buffer.data(), buffer.size(), bytesReceived);
            lastSocketError = m_clientTcpSocket->GetLastSocketError();
            if (ecResult != ErrorCodes::Success)
            {
                if (ecResult == ErrorCodes::ReceiveConnectionClosed)
                {
                    m_clientTcpSocket->Shutdown();
                    m_clientTcpSocket.reset();
                    ServiceLocator::Logger().LogInfo(
                        "Disconnected from server (connection closed by remote host)");
                    return ErrorCodes::ReceiveConnectionClosed;
                }
                else if (ecResult != ErrorCodes::ReceiveWouldBlock)
                {
                    Utils::LogSocketError("ReceiveFromClientTcp: Receive", ecResult, lastSocketError);
                    
                    m_clientTcpSocket->Shutdown();
                    m_clientTcpSocket.reset();
                    ServiceLocator::Logger().LogInfo(
                        "Disconnected from server (connection closed by remote host)");
                    
                    return messagesReceivedThisFrame > 0 
                        ? ErrorCodes::PartialSuccess : ErrorCodes::ReceiveFailed;
                }

                break;
            }

            buffer.resize(bytesReceived);
            m_clientReceivedTcpThisTick.emplace_back(std::move(buffer));

            messagesReceivedThisFrame++;
        }
        
        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::ClientSendTcp()
    {
        ErrorCodes ecResult;
        int lastSocketError;

        int bytesSent;

        for (const std::vector<uint8_t>& message : m_clientTcpSendQueue)
        {
            ecResult = m_clientTcpSocket->Send(message.data(), message.size(), bytesSent);
            lastSocketError = m_clientTcpSocket->GetLastSocketError();
            if (ecResult == ErrorCodes::SendConnectionClosed)
            {
                m_clientTcpSocket->Shutdown();
                m_clientTcpSocket.reset();
                ServiceLocator::Logger().LogInfo(
                    "Disconnected from server (connection closed by remote host)");

                m_clientTcpSendQueue.clear();
                return ErrorCodes::ReceiveConnectionClosed;
            }
            else if (ecResult != ErrorCodes::Success)
            {
                Utils::LogSocketError("SendToServerTcp: Send", ecResult, lastSocketError);

                m_clientTcpSocket->Shutdown();
                m_clientTcpSocket.reset();
                ServiceLocator::Logger().LogInfo(
                    "Disconnected from server (connection closed by remote host)");

                m_clientTcpSendQueue.clear();
                return ecResult;
            }
        }

        m_clientTcpSendQueue.clear();
        return ErrorCodes::Success;
    }
}