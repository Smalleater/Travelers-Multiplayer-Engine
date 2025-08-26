#include "TME/NetworkEngine.hpp"

#include <iostream>

#include "ServiceManager.hpp"
#include "NetworkManager.hpp"
#include "ServiceLocator.hpp"

namespace tme
{
    void* Network::m_networkManager = nullptr;

    namespace
    {
        ErrorCodes ValidateEngineState()
        {
            if(!Network::Engine::IsInitialized())
            {
                std::cerr << "[TME ERROR] NetworkEngine is not initialized" << std::endl;
                return ErrorCodes::EngineNotInitialized;
            }

            return ErrorCodes::Success;
        }

        ErrorCodes ValidateServerState()
        {
            ErrorCodes ecResult = ValidateEngineState();
            if (ecResult != ErrorCodes::Success)
            {
                return ecResult;
            }

            if (!Network::Server::IsStarted())
            {
                ServiceLocator::Logger().LogError("Server is not started");
                return ErrorCodes::ServerNotStarted;
            }

            return ErrorCodes::Success;
        }

        ErrorCodes ValidateClientState()
        {
            ErrorCodes ecResult = ValidateEngineState();
            if (ecResult != ErrorCodes::Success)
            {
                return ecResult;
            }

            if (!Network::Client::IsConnected())
            {
                ServiceLocator::Logger().LogError("Client is not connected");
                return ErrorCodes::ClientNotConnected;
            }

            return ErrorCodes::Success;
        }
    }

    // Engine

    ErrorCodes Network::Engine::Init()
    {
        if (IsInitialized())
        {
            ServiceLocator::Logger().LogWarning("Network Engine is already initialized");
            return ErrorCodes::AlreadyInitialized;
        }

        ErrorCodes ecResult;

        ecResult = ServiceManager::Init();
        if (ecResult != ErrorCodes::Success)
        {
            std::cerr << "[TME ERROR] ServiceManager initialization failed with error code: " 
                << static_cast<int>(ecResult) << std::endl;
            return ecResult;
        }

        m_networkManager = new NetworkManager();

        ServiceLocator::Logger().LogInfo("Network Engine has been started successfully");
        return ErrorCodes::Success;
    }

    ErrorCodes Network::Engine::ShutDown()
    {
        ErrorCodes ecResult;

        ServiceLocator::ThreadManager().Shutdown();
        ServiceLocator::Logger().LogInfo("ThreadManager was shutdown successfully");

        delete static_cast<NetworkManager*>(m_networkManager);
        m_networkManager = nullptr;

        ecResult = ServiceManager::ShutDown();
        if (ecResult != ErrorCodes::Success)
        {
            std::cerr << "[TME ERROR] ServiceManager shutdown failed with error code: " 
                << static_cast<int>(ecResult) << std::endl;
            return ErrorCodes::PartialSuccess;
        }

        return ErrorCodes::Success;
    }

    bool Network::Engine::IsInitialized()
    {
        return m_networkManager != nullptr;
    }

    ErrorCodes Network::Engine::EnsurInitialized()
    {
        if (m_networkManager != nullptr)
        {
            return ErrorCodes::Success;
        }

        return Init();
    }

    ErrorCodes Network::Engine::BeginUpdate()
    {
        ErrorCodes ecResult = ValidateEngineState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        return static_cast<NetworkManager*>(m_networkManager)->BeginUpdate();
    }

    ErrorCodes Network::Engine::EndUpdate()
    {
        ErrorCodes ecResult = ValidateEngineState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        return static_cast<NetworkManager*>(m_networkManager)->EndUpdate();
    }

    // Server

    ErrorCodes Network::Server::Start(uint16_t port)
    {
        if (IsStarted())
        {
            ServiceLocator::Logger().LogWarning("Server is already started");
            return ErrorCodes::Success;
        }

        ErrorCodes ecResult = ValidateEngineState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        return static_cast<NetworkManager*>(m_networkManager)->StartServer(port);
    }

    bool Network::Server::IsStarted()
    {
        if (!Engine::IsInitialized())
        {
            return false;
        }

        return static_cast<NetworkManager*>(m_networkManager)->HasServer();
    }

    ErrorCodes Network::Server::GetNewClientThisTick(std::vector<uint32_t>& outNetworkIds)
    {
        ErrorCodes ecResult = ValidateServerState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        const std::unique_ptr<ServerCore>& server = static_cast<NetworkManager*>(m_networkManager)->GetServer();
        outNetworkIds = server->GetNewClientsThisTick();

        return ErrorCodes::Success;
    }

    ErrorCodes Network::Server::GetReceivedReliableThisTick(std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& outMessages)
    {
        ErrorCodes ecResult = ValidateServerState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        const std::unique_ptr<ServerCore>& server = static_cast<NetworkManager*>(m_networkManager)->GetServer();
        outMessages = server->GetReceivedTcpThisTick();

        return ErrorCodes::Success;
    }

    ErrorCodes Network::Server::SendReliableTo(uint32_t networkId, const std::vector<uint8_t>& message)
    {
        ErrorCodes ecResult = ValidateServerState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        const std::unique_ptr<ServerCore>& server = static_cast<NetworkManager*>(m_networkManager)->GetServer();
        server->AddMessageToTcpPerClientSendQueue(networkId, message);

        return ErrorCodes::Success;
    }

    ErrorCodes Network::Server::SendReliableToAll(const std::vector<uint8_t>& message)
    {
        ErrorCodes ecResult = ValidateServerState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        const std::unique_ptr<ServerCore>& server = static_cast<NetworkManager*>(m_networkManager)->GetServer();
        server->AddMessageToTcpBroadcastSendQueue(message);

        return ErrorCodes::Success;
    }

    // Client

    ErrorCodes Network::Client::ConnectTo(const std::string& address, uint16_t port)
    {
        if (IsConnected())
        {
            ServiceLocator::Logger().LogWarning("Client is already connected");
            return ErrorCodes::ClientAlreadyConnected;
        }

        ErrorCodes ecResult = ValidateEngineState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        return static_cast<NetworkManager*>(m_networkManager)->ConnectClient(address, port);
    }

    bool Network::Client::IsConnected()
    {
        if (!Engine::IsInitialized())
        {
            return false;
        }

        return static_cast<NetworkManager*>(m_networkManager)->HasClient();
    }

    ErrorCodes Network::Client::GetReceivedReliableThisTick(std::vector<std::vector<uint8_t>>& outMessages)
    {
        ErrorCodes ecResult = ValidateClientState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        const std::unique_ptr<ClientCore>& client = static_cast<NetworkManager*>(m_networkManager)->GetClient();
        outMessages = client->GetReceivedTcpThisTick();

        return ErrorCodes::Success;
    }

    ErrorCodes Network::Client::SendReliable(const std::vector<uint8_t>& message)
    {
        ErrorCodes ecResult = ValidateClientState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        const std::unique_ptr<ClientCore>& client = static_cast<NetworkManager*>(m_networkManager)->GetClient();
        client->AddMessageToTcpSendQueue(message);

        return ErrorCodes::Success;
    }
}