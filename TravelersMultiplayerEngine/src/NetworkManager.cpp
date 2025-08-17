#include "NetworkManager.hpp"

#include "ServiceLocator.hpp"
#include "Utils.hpp"

namespace tme
{
    // Handles accepting new clients on the server socket
    // Returns PartialSuccess if some errors occured, Success otherwise
    ErrorCodes NetworkManager::UpdateServer()
    {
        bool hadError = false;

        const uint8_t maxAcceptPerFrame = 16;
        uint8_t acceptedThisFrame = 0;

        std::unique_ptr<ISocket> outClient;
        std::unique_ptr<TcpSocket> tcpClient;

        ErrorCodes ecResult;
        int lastError;

        uint32_t networkId;

        while (acceptedThisFrame < maxAcceptPerFrame)
        {
            ecResult = m_serverTcpSocket->Accept(outClient);
            lastError = m_serverTcpSocket->GetLastSocketError();

            if (ecResult != ErrorCodes::Success)
            {
                if (lastError != WOULD_BLOCK_ERROR)
                {
                    ServiceLocator::Logger().LogError("UpdateServer: Failed to accept new client with code: "
                        + std::to_string(static_cast<int>(ecResult)) + " Last socket error: "
                        + std::to_string(lastError));        
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
                continue;
            }
            
            networkId = m_nextNetworkId++;
            m_clients[networkId] = std::move(tcpClient);
            ServiceLocator::Logger().LogInfo("New client connected, with network id: " 
                + std::to_string(networkId));

            acceptedThisFrame++;
        }

        return hadError ? ErrorCodes::PartialSuccess : ErrorCodes::Success;
    }

    // Handles client-side network updates (currently sub)
    ErrorCodes NetworkManager::UpdateClient()
    {
        return ErrorCodes::Success;
    }

    bool NetworkManager::HasServerSocket() const
    {
        return m_serverTcpSocket != nullptr;
    }

    bool NetworkManager::HasClientSocket() const
    {
        return m_clientTcpSocket != nullptr;
    }

    // Starts the server, binds and listens on the given port
    // Initializes WSA on Windows if needed
    ErrorCodes NetworkManager::StartServer(uint16_t port)
    {
        ErrorCodes ecResult;
        int lastError;

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
                lastError = m_wsa->GetWsaStartupResult();
                if (ecResult != ErrorCodes::Success || lastError != 0)
                {
                    ServiceLocator::Logger().LogError("StartServer: WSA initialization failed with code: " 
                        + std::to_string(static_cast<int>(ecResult)) + " Wsa startup error: " 
                        + std::to_string(lastError));
                    return ecResult;
                }
            }
        #endif

        m_serverTcpSocket = std::make_unique<TcpSocket>();

        ecResult = m_serverTcpSocket->Bind(port);
        lastError = m_serverTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("StartServer: Bind failed with code: " 
                + std::to_string(static_cast<int>(ecResult)) + " Last socket error: " 
                + std::to_string(lastError));
            return ecResult;
        }

        ecResult = m_serverTcpSocket->Listen();
        lastError = m_serverTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("StartServer: Listen failed with code: " 
                + std::to_string(static_cast<int>(ecResult)) + " Last socket error: " 
                + std::to_string(lastError));
            return ecResult;
        }

        ecResult = m_serverTcpSocket->SetBlocking(false);
        lastError = m_serverTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("StartServer: SetBlocking failed with code: " 
                + std::to_string(static_cast<int>(ecResult)) + " Last socket error: " 
                + std::to_string(lastError));
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    // Starts the client and connects to the given address and port
    // Initializes WSA on Windows if needed
    ErrorCodes NetworkManager::StartClient(const std::string& address, uint16_t port)
    {
        ErrorCodes ecResult;
        int lastError;

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
                lastError = m_wsa->GetWsaStartupResult();
                if (ecResult != ErrorCodes::Success || lastError != 0)
                {
                    ServiceLocator::Logger().LogError("StartClient: WSA initialization failed with code: " 
                        + std::to_string(static_cast<int>(ecResult)) + " Wsa startup error: " 
                        + std::to_string(lastError));
                    return ecResult;
                }
            }
        #endif

        m_clientTcpSocket = std::make_unique<TcpSocket>();

        ecResult = m_clientTcpSocket->Connect(address, port);
        lastError = m_clientTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("StartClient: Connect failed with code: " 
                + std::to_string(static_cast<int>(ecResult)) + " Last socket error: " 
                + std::to_string(lastError));
            return ecResult;
        }

        ecResult = m_clientTcpSocket->SetBlocking(false);
        lastError = m_clientTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("StartClient: SetBlocking failed with code: " 
                + std::to_string(static_cast<int>(ecResult)) + " Last socket error: " 
                + std::to_string(lastError));
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    // Main update function, calls server/client update as needed
    ErrorCodes NetworkManager::Update()
    {
        bool hadError = false;
        ErrorCodes ecResult;

        if (m_serverTcpSocket != nullptr)
        {
            ecResult = UpdateServer();
            if (ecResult != ErrorCodes::Success)
            {
                if (ecResult != ErrorCodes::PartialSuccess)
                {
                    return ecResult;
                }

                hadError = true;
            }
        }

        if (m_clientTcpSocket != nullptr)
        {
            ecResult = UpdateClient();
            if (ecResult != ErrorCodes::Success)
            {
                if (ecResult != ErrorCodes::PartialSuccess)
                {
                    return ecResult;
                }

                hadError = true;
            }
        }

        return hadError ? ErrorCodes::PartialSuccess : ErrorCodes::Success;
    }

    // Sends data to server via TCP
    ErrorCodes NetworkManager::SendToServerTcp(const std::vector<uint8_t>& data)
    {
        if (m_clientTcpSocket == nullptr)
        {
            ServiceLocator::Logger().LogError(
                "SendToServerTcp: TCP client socket not initialized in NetworkManager");
            return ErrorCodes::ClientNotInitialized;
        }

        ErrorCodes ecResult;
        int lastError;

        int bytesSent;

        ecResult = m_clientTcpSocket->Send(data.data(), data.size(), bytesSent);
        lastError = m_clientTcpSocket->GetLastSocketError();
        if (ecResult == ErrorCodes::SendConnectionClosed)
        {
            m_clientTcpSocket->Shutdown();
            m_clientTcpSocket.reset();
            ServiceLocator::Logger().LogInfo(
                "Disconnected from server (connection closed by remote host)");

            return ErrorCodes::ReceiveConnectionClosed;
        }
        else if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("SendToServerTcp: Send failed with code: " 
                + std::to_string(static_cast<int>(ecResult)) + " Last socket error: " 
                + std::to_string(lastError));

            m_clientTcpSocket->Shutdown();
            m_clientTcpSocket.reset();
            ServiceLocator::Logger().LogInfo(
                "Disconnected from server (connection closed by remote host)");

            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::SendToClientTcp(const std::vector<uint8_t>& data, uint32_t networkId)
    {
        if (m_clients.find(networkId) == m_clients.end())
        {
            ServiceLocator::Logger().LogError("SendToClientTcp: No TCP client found for networkId" 
                + std::to_string(networkId));
            return ErrorCodes::ClientNotFound;
        }

        ErrorCodes ecResult;
        int lastError;

        int bytesSent;

        ecResult = m_clients[networkId]->Send(data.data(), data.size(), bytesSent);
        lastError = m_clients[networkId]->GetLastSocketError();
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
            ServiceLocator::Logger().LogError("SendToClientTcp: Send failed with code: " 
                + std::to_string(static_cast<int>(ecResult)) + " Last socket error: " 
                + std::to_string(lastError));

            m_clients[networkId]->Shutdown();
            m_clients.erase(networkId);
            ServiceLocator::Logger().LogInfo("Client with id: " + std::to_string(networkId) 
                + " disconnected");

            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::SendToAllClientTcp(const std::vector<uint8_t>& data)
    {
        if (m_clients.empty())
        {
            return ErrorCodes::Success;
        }

        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        for (std::pair<const uint32_t, std::unique_ptr<TcpSocket>>& clientPair : m_clients)
        {
            ecResult = SendToClientTcp(data, clientPair.first);
            if (ecResult != ErrorCodes::Success)
            {
                hadError = true;
            }
            else
            {
                hadSuccess = true;
            }
        }

        if (hadSuccess && !hadError)
        {
            return ErrorCodes::Success;
        }
        else if (hadSuccess && hadError)
        {
            return ErrorCodes::PartialSuccess;
        }
        else
        {
            return ErrorCodes::Failure;
        }
    }

    // Receives all available messages from all connected clients
    // Removes clients that have disconnected
    ErrorCodes NetworkManager::ReceiveFromAllClientsTcp(
        std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& outMessages)
    {
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
        int lastError;

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
                lastError = clientSocket->GetLastSocketError();
                if (ecResult != ErrorCodes::Success)
                {
                    if (ecResult == ErrorCodes::ReceiveConnectionClosed)
                    {
                        clientsToRemove.push_back(networkId);
                    }
                    else if (ecResult != ErrorCodes::ReceiveWouldBlock)
                    {
                        ServiceLocator::Logger().LogError("ReceiveAllFromServerTcp: Receive failed with code: " 
                            + std::to_string(static_cast<int>(ecResult)) + " Last socket error: " 
                            + std::to_string(lastError));

                        clientsToRemove.push_back(networkId);

                        hadError = true;
                    }
                    else
                    {
                        hadSuccess = true;
                    }

                    break;
                }
                else
                {
                    hadSuccess = true;
                }

                buffer.resize(bytesReceived);
                outMessages.emplace_back(networkId, std::move(buffer));

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

        if (hadSuccess && !hadError)
        {
            return ErrorCodes::Success;
        }
        else if (hadSuccess && hadError)
        {
            return ErrorCodes::PartialSuccess;
        }
        else
        {
            return ErrorCodes::Failure;
        }
    }

    ErrorCodes NetworkManager::ReceiveFromServerTcp(std::vector<std::vector<uint8_t>>& outMessages)
    {
        const uint8_t maxMessagesPerFrame = 32;
        uint8_t messagesReceivedThisFrame = 0;

        int bytesReceived;
        std::vector<uint8_t> buffer(4096);

        ErrorCodes ecResult;
        int lastError;

        while (messagesReceivedThisFrame < maxMessagesPerFrame)
        {
            bytesReceived = 0;
            buffer.resize(4096);

            ecResult = m_clientTcpSocket->Receive(buffer.data(), buffer.size(), bytesReceived);
            lastError = m_clientTcpSocket->GetLastSocketError();
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
                    ServiceLocator::Logger().LogError("ReceiveFromClientTcp: Receive failed with code: " 
                            + std::to_string(static_cast<int>(ecResult)) + " Last socket error: " 
                            + std::to_string(lastError));
                    
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
            outMessages.emplace_back(std::move(buffer));

            messagesReceivedThisFrame++;
        }
        
        return ErrorCodes::Success;
    }
}