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

        /// @brief Updates the network engine state (processes network events, etc.).
        /// @return ErrorCodes Result of the update operation.
        static TME_API ErrorCodes Update();

        /// @brief Shuts down the network engine and releases resources.
        /// @return ErrorCodes Result of the shutdown operation.
        static TME_API ErrorCodes ShutDown();

        /// @brief Ensures that the network engine is initialized.
        ///
        /// Call Init() if the engine is not already initialized.
        ///
        /// @return ErrorCodes Result of the initialization check or operation. 
        static TME_API ErrorCodes EnsureInitialized();

        /// @brief Checks if the network engine is currently initialized.
        /// @return true if initialized, false otherwise.
        static TME_API bool IsInitialized();

        /// @brief Starts the network engine in server mode.
        /// @return ErrorCodes Result of the server start operation.
        static TME_API ErrorCodes StartServer(uint16_t port);

        /// @brief Starts the network engine in client mode.
        /// @return ErrorCodes Result of the client start operation.
        static TME_API ErrorCodes StartClient(const std::string& address, uint16_t port);

        /// @brief Sends data reliably to the server.
        /// @param data The data buffer to send.
        /// @return ErrorCodes Result of the send operation.
        static TME_API ErrorCodes SendToServerReliable(const std::vector<uint8_t>& data);

        /// @brief Sends data unreliably to the server (may be lost or arrive out of order).
        /// @param data The data buffer to send.
        /// @return ErrorCodes Result of the send operation.
        static TME_API ErrorCodes SendToServerUnreliable(const std::vector<uint8_t>& data);

        /// @brief Sends data reliably to a specific client.
        /// @param data The data buffer to send.
        /// @param clientId The unique identifier of the target client.
        /// @return ErrorCodes Result of the send operation.
        static TME_API ErrorCodes SendToClientReliable(const std::vector<uint8_t>& data, uint32_t clientId);

        /// @brief Sends data unreliably to a specific client (may be lost or arrive out of order).
        /// @param data The data buffer to send.
        /// @param clientId The unique identifier of the target client.
        /// @return ErrorCodes Result of the send operation.
        static TME_API ErrorCodes SendToClientUnreliable(const std::vector<uint8_t>& data, uint32_t clientId);
    };    
}

#endif