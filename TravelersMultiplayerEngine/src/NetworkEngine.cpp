#include "TME/NetworkEngine.hpp"

#include <iostream>

#include "Core/EngineCore.hpp"
#include "ServiceManager.hpp"
#include "ServiceLocator.hpp"

namespace tme
{
    void* Network::m_engineCore = nullptr;

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

        m_engineCore = new EngineCore();

        ServiceLocator::Logger().LogInfo("Network Engine has been started successfully");
        return ErrorCodes::Success;
    }

    ErrorCodes Network::Engine::ShutDown()
    {
        ErrorCodes ecResult;

        Client::Disconnect();
        Server::Stop();

        ServiceLocator::ThreadManager().Shutdown();
        ServiceLocator::Logger().LogInfo("ThreadManager was shutdown successfully");

        delete static_cast<EngineCore*>(m_engineCore);
        m_engineCore = nullptr;

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
        return m_engineCore != nullptr;
    }

    ErrorCodes Network::Engine::EnsurInitialized()
    {
        if (m_engineCore != nullptr)
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

        return static_cast<EngineCore*>(m_engineCore)->BeginUpdate();
    }

    ErrorCodes Network::Engine::EndUpdate()
    {
        ErrorCodes ecResult = ValidateEngineState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        return static_cast<EngineCore*>(m_engineCore)->EndUpdate();
    }

    // Server

    ErrorCodes Network::Server::Start(uint16_t port)
    {
        ErrorCodes ecResult = ValidateEngineState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        if (IsStarted())
        {
            ServiceLocator::Logger().LogWarning("Server is already started");
            return ErrorCodes::Success;
        }

        return static_cast<EngineCore*>(m_engineCore)->StartServer(port);
    }

    ErrorCodes Network::Server::Stop()
    {
        ErrorCodes ecResult = ValidateEngineState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        return static_cast<EngineCore*>(m_engineCore)->StopServer();
    }

    bool Network::Server::IsStarted()
    {
        if (!Engine::IsInitialized())
        {
            return false;
        }

        return static_cast<EngineCore*>(m_engineCore)->IsServerStarted();
    }

    ErrorCodes Network::Server::GetNewClientThisTick(std::vector<uint32_t>& outNetworkIds)
    {
        ErrorCodes ecResult = ValidateServerState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        const std::unique_ptr<ServerCore>& server = static_cast<EngineCore*>(m_engineCore)->GetServer();
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

        const std::unique_ptr<ServerCore>& server = static_cast<EngineCore*>(m_engineCore)->GetServer();
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

        const std::unique_ptr<ServerCore>& server = static_cast<EngineCore*>(m_engineCore)->GetServer();
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

        const std::unique_ptr<ServerCore>& server = static_cast<EngineCore*>(m_engineCore)->GetServer();
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

        return static_cast<EngineCore*>(m_engineCore)->ConnectClient(address, port);
    }

    ErrorCodes Network::Client::Disconnect()
    {
        ErrorCodes ecResult = ValidateEngineState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

         return static_cast<EngineCore*>(m_engineCore)->DisconnectClient();
    }

    bool Network::Client::IsConnected()
    {
        if (!Engine::IsInitialized())
        {
            return false;
        }

        return static_cast<EngineCore*>(m_engineCore)->IsClientConnected();
    }

    ErrorCodes Network::Client::GetReceivedReliableThisTick(std::vector<std::vector<uint8_t>>& outMessages)
    {
        ErrorCodes ecResult = ValidateClientState();
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        const std::unique_ptr<ClientCore>& client = static_cast<EngineCore*>(m_engineCore)->GetClient();
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

        const std::unique_ptr<ClientCore>& client = static_cast<EngineCore*>(m_engineCore)->GetClient();
        client->AddMessageToTcpSendQueue(message);

        return ErrorCodes::Success;
    }
}