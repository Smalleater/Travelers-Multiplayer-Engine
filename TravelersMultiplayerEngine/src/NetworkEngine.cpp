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

        try
        {
            m_networkManager = new NetworkManager();
        }
        catch(const std::bad_alloc& e)
        {
            ServiceLocator::Logger().LogError("Failed to allocate NetworkManager: " + std::string(e.what()));

            ecResult = ServiceManager::ShutDown();
            if (ecResult != ErrorCodes::Success)
            {
                std::cerr << "[TME ERROR] ServiceManager shutdown failed with error code: " 
                    << static_cast<int>(ecResult) << std::endl;
            }

            return ErrorCodes::OutOfMemory;
        }

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

        return static_cast<NetworkManager*>(m_networkManager)->HasServerSocket();
    }

    ErrorCodes Network::Server::GetNewClientThisTick(std::vector<uint32_t>& outNetworkIds)
    {
        ErrorCodes ecResult = ValidateServerState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        outNetworkIds = static_cast<NetworkManager*>(m_networkManager)->GetNewClientsThisTick();

        return ErrorCodes::Success;
    }

    ErrorCodes Network::Server::GetReceivedReliableThisTick(std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& outMessages)
    {
        ErrorCodes ecResult = ValidateServerState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        outMessages = static_cast<NetworkManager*>(m_networkManager)->GetServerReceivedTcpThisTick();

        return ErrorCodes::Success;
    }

    ErrorCodes Network::Server::SendReliableTo(uint32_t networkId, const std::vector<uint8_t>& message)
    {
        ErrorCodes ecResult = ValidateServerState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        static_cast<NetworkManager*>(m_networkManager)->AddMessageToServerTcpPerClientSendQueue(networkId, message);

        return ErrorCodes::Success;
    }

    ErrorCodes Network::Server::SendReliableToAll(const std::vector<uint8_t>& message)
    {
        ErrorCodes ecResult = ValidateServerState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        static_cast<NetworkManager*>(m_networkManager)->AddMessageToServerTcpBroadcastSendQueue(message);

        return ErrorCodes::Success;
    }

    // Client

    ErrorCodes Network::Client::Connect(const std::string& address, uint16_t port)
    {
        if (IsConnected())
        {
            ServiceLocator::Logger().LogWarning("Client is already connected");
            return ErrorCodes::AlreadyConnected;
        }

        ErrorCodes ecResult = ValidateEngineState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        return static_cast<NetworkManager*>(m_networkManager)->StartClient(address, port);
    }

    bool Network::Client::IsConnected()
    {
        if (!Engine::IsInitialized())
        {
            return false;
        }

        return static_cast<NetworkManager*>(m_networkManager)->HasClientSocket();
    }

    ErrorCodes Network::Client::GetReceivedReliableThisTick(std::vector<std::vector<uint8_t>>& outMessages)
    {
        ErrorCodes ecResult = ValidateClientState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        outMessages = static_cast<NetworkManager*>(m_networkManager)->GetClientReceivedTcpThisTick();

        return ErrorCodes::Success;
    }

    ErrorCodes Network::Client::SendReliable(const std::vector<uint8_t>& message)
    {
        ErrorCodes ecResult = ValidateClientState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        static_cast<NetworkManager*>(m_networkManager)->AddMessageToClientTcpSendQueue(message);

        return ErrorCodes::Success;
    }
}