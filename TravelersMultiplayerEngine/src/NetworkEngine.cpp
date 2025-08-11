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

    // Initialize all required services and create the NetworkManager instance
    ErrorCodes NetworkEngine::Init()
    {
        ErrorCodes result;

        result = ServiceManager::Init();
        if (result != ErrorCodes::Success)
        {
            // Throw exception if service initialization fails
            throw std::runtime_error("Service initialization failed");
        }

        // Instantiate the network manager
        m_networkManager = new NetworkManager();

        m_initialized = true;

        // Log successful startup
        ServiceLocator::Logger().LogInfo("Network Engine has been started successfully");
        return ErrorCodes::Success;
    }

    // Updates the network manager
    ErrorCodes NetworkEngine::Update()
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }
        
        return static_cast<NetworkManager*>(m_networkManager)->Update();
    }

    // Shut down all services and cleanup resources
    ErrorCodes NetworkEngine::ShutDown()
    {
        ErrorCodes result = ErrorCodes::Success;

        // Shutdown thread manager service
        ServiceLocator::ThreadManager().Shutdown();
        ServiceLocator::Logger().LogInfo("ThreadManager was shutdown successfully");

        // Delete network manager instance
        delete static_cast<NetworkManager*>(m_networkManager);
        m_networkManager = nullptr;

        // Shutdown services and check for errors
        if (ServiceManager::ShutDown() != ErrorCodes::Success)
        {
            result = ErrorCodes::CompletedWithErrors;
        }

        m_initialized = false;

        return result;
    }

    // Ensure the engine is initialized, initialize if necessary
    ErrorCodes NetworkEngine::EnsureInitialized()
    {
        if (m_initialized)
        {
            return ErrorCodes::Success;
        }

        return Init();
    }

    // Return whether the engine is initialized
    bool NetworkEngine::IsInitialized()
    {
        return m_initialized;
    }

    // Start the engine as a server
    ErrorCodes NetworkEngine::StartServer(uint16_t port)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }

        return static_cast<NetworkManager*>(m_networkManager)->StartServer(port);
    }

    // Start the engine as a client
    ErrorCodes NetworkEngine::StartClient(const std::string& address, uint16_t port)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }

        return static_cast<NetworkManager*>(m_networkManager)->StartClient(address, port);
    }

    ErrorCodes NetworkEngine::SendToServerReliable(const std::vector<uint8_t>& data)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }

        return static_cast<NetworkManager*>(m_networkManager)->SendToServerTcp(data);
    }

    ErrorCodes NetworkEngine::ReceiveAllReliableFromServer(
        std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& outMessages)
    {
        if (m_networkManager == nullptr)
        {
            return ErrorCodes::NetworkEngineNotInitialized;
        }

        return static_cast<NetworkManager*>(m_networkManager)->ReceiveAllFromServerTcp(outMessages);
    }
}