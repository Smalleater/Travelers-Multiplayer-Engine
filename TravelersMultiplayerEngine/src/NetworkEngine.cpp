#include "TME/NetworkEngine.hpp"

#include <stdexcept>

#include "ServiceManager.hpp"
#include "NetworkManager.hpp"
#include "ServiceLocator.hpp"

namespace tme
{
    // Static members initialization
    bool NetworkEngine::m_initialized = false;
    void* NetworkEngine::m_networkManager = nullptr;

    // Initializes the network engine and its dependencies
    ErrorCodes NetworkEngine::Init()
    {
        ErrorCodes result;

        result = ServiceManager::Init();
        if (result != ErrorCodes::Success)
        {
            throw std::runtime_error("Service initialization failed");
        }

        m_networkManager = new NetworkManager();

        m_initialized = true;

        ServiceLocator::Logger().LogInfo("Network Engine has been started successfully");
        return ErrorCodes::Success;
    }

    ErrorCodes NetworkEngine::BeginUpdate()
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }
        
        return static_cast<NetworkManager*>(m_networkManager)->BeginUpdate();
    }

    ErrorCodes NetworkEngine::EndUpdate()
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }
        
        return static_cast<NetworkManager*>(m_networkManager)->EndUpdate();
    }

    // Shuts down the network engine and cleans up resources
    ErrorCodes NetworkEngine::ShutDown()
    {
        ErrorCodes result = ErrorCodes::Success;

        ServiceLocator::ThreadManager().Shutdown();
        ServiceLocator::Logger().LogInfo("ThreadManager was shutdown successfully");

        delete static_cast<NetworkManager*>(m_networkManager);
        m_networkManager = nullptr;

        if (ServiceManager::ShutDown() != ErrorCodes::Success)
        {
            result = ErrorCodes::CompletedWithErrors;
        }

        m_initialized = false;

        return result;
    }

    // Ensures the network engine is initialized before use
    ErrorCodes NetworkEngine::EnsureInitialized()
    {
        if (m_initialized)
        {
            return ErrorCodes::Success;
        }

        return Init();
    }

    ErrorCodes NetworkEngine::GetNewClientThisTick(std::vector<uint32_t>& outNetworkIds)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }
        else if (!static_cast<NetworkManager*>(m_networkManager)->HasServerSocket())
        {
            return ErrorCodes::NetworkServerNotStarted;
        }

        outNetworkIds = static_cast<NetworkManager*>(m_networkManager)->GetNewClientsThisTick();

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkEngine::GetServerReceivedReliableThisTick(
        std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& outMessages)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }
        else if (!static_cast<NetworkManager*>(m_networkManager)->HasServerSocket())
        {
            return ErrorCodes::NetworkServerNotStarted;
        }

        outMessages = static_cast<NetworkManager*>(m_networkManager)->GetServerReceivedTcpThisTick();

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkEngine::GetClientReceivedReliableThisTick(std::vector<std::vector<uint8_t>>& outMessages)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }
        else if (!static_cast<NetworkManager*>(m_networkManager)->HasClientSocket())
        {
            return ErrorCodes::NetworkClientNotConnected;
        }

        outMessages = static_cast<NetworkManager*>(m_networkManager)->GetClientReceivedTcpThisTick();

        return ErrorCodes::Success;
    }

    // Returns whether the network engine is initialized
    bool NetworkEngine::IsInitialized()
    {
        return m_initialized;
    }

    bool NetworkEngine::IsServerStarted()
    {
        if (m_networkManager == nullptr)
        {
            return false;
        }

        return static_cast<NetworkManager*>(m_networkManager)->HasServerSocket();
    }

    bool NetworkEngine::IsClientConnected()
    {
        if (m_networkManager == nullptr)
        {
            return false;
        }

        return static_cast<NetworkManager*>(m_networkManager)->HasClientSocket();
    }

    // Starts the server on the specified port
    ErrorCodes NetworkEngine::StartServer(uint16_t port)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }

        return static_cast<NetworkManager*>(m_networkManager)->StartServer(port);
    }

    // Starts the client and connects to the specified address and port
    ErrorCodes NetworkEngine::StartClient(const std::string& address, uint16_t port)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }

        return static_cast<NetworkManager*>(m_networkManager)->StartClient(address, port);
    }

    ErrorCodes NetworkEngine::SendToClientReliable(uint32_t networkId, const std::vector<uint8_t>& data)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }
        else if (!static_cast<NetworkManager*>(m_networkManager)->HasServerSocket())
        {
            return ErrorCodes::NetworkServerNotStarted;
        }

        static_cast<NetworkManager*>(m_networkManager)->AddMessageToServerTcpPerClientSendQueue(networkId, data);

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkEngine::SendToAllClientReliable(const std::vector<uint8_t>& data)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }
        else if (!static_cast<NetworkManager*>(m_networkManager)->HasServerSocket())
        {
            return ErrorCodes::NetworkServerNotStarted;
        }

        static_cast<NetworkManager*>(m_networkManager)->AddMessageToServerTcpBroadcastQueue(data);

        return ErrorCodes::Success;
    }

    // Sends data reliably to the server (TCP)
    ErrorCodes NetworkEngine::SendToServerReliable(const std::vector<uint8_t>& data)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }
        else if (!static_cast<NetworkManager*>(m_networkManager)->HasClientSocket())
        {
            return ErrorCodes::NetworkClientNotConnected;
        }

        return static_cast<NetworkManager*>(m_networkManager)->SendToServerTcp(data);
    }
}