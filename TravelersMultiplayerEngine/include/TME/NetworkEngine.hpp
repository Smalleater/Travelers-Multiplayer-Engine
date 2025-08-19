#ifndef TME_NETWORK_ENGINE_HPP
#define TME_NETWORK_ENGINE_HPP

#include <string>
#include <vector>
#include <cstdint>

#include "TME/Export.hpp"

#include "TME/ErrorCodes.hpp"

namespace tme
{
    /// @brief Main class responsible for managing the network engine lifecycle.
    ///
    /// Provides initialization, shutdown, and controls to start server/client modes.
    class NetworkEngine
    {
    private:
        /// @brief Indicates whether the network engine has been initialized.
        static bool m_initialized;

        /// @brief Pointer to the internal network manager instance.
        static void* m_networkManager;

    public:
        /// @brief Initializes the network engine and its services.
        /// @return ErrorCodes Result of the initialization operation.
        static TME_API ErrorCodes Init();

        static TME_API ErrorCodes BeginUpdate();

        static TME_API ErrorCodes EndUpdate();

        /// @brief Shuts down the network engine and releases resources.
        /// @return ErrorCodes Result of the shutdown operation.
        static TME_API ErrorCodes ShutDown();

        /// @brief Ensures that the network engine is initialized.
        ///
        /// Call Init() if the engine is not already initialized.
        ///
        /// @return ErrorCodes Result of the initialization check or operation. 
        static TME_API ErrorCodes EnsureInitialized();

        static TME_API ErrorCodes GetNewClientThisTick(std::vector<uint32_t>& outNetworkIds);

        static TME_API ErrorCodes GetServerReceivedReliableThisTick(
            std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& outMessages);

        /// @brief Checks if the network engine is currently initialized.
        /// @return true if initialized, false otherwise.
        static TME_API bool IsInitialized();

        static TME_API bool IsServerStarted();

        static TME_API bool IsClientConnected();

        /// @brief Starts the network engine in server mode.
        /// @return ErrorCodes Result of the server start operation.
        static TME_API ErrorCodes StartServer(uint16_t port);

        /// @brief Starts the network engine in client mode.
        /// @return ErrorCodes Result of the client start operation.
        static TME_API ErrorCodes StartClient(const std::string& address, uint16_t port);

        static TME_API ErrorCodes SendToClientReliable(const std::vector<uint8_t>& data, uint32_t networkId);

        static TME_API ErrorCodes SendToAllClientReliable(const std::vector<uint8_t>& data);

        /// @brief Sends data reliably to the server.
        /// @param data The data buffer to send.
        /// @return ErrorCodes Result of the send operation.
        static TME_API ErrorCodes SendToServerReliable(const std::vector<uint8_t>& data);

        static TME_API ErrorCodes ReceiveAllReliableFromServer(std::vector<std::vector<uint8_t>>& outMessages);
    };    
}

#endif