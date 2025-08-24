#include <vector>
#include <string>
#include <cstdint>

#include "TME/Export.hpp"
#include "TME/ErrorCodes.hpp"

namespace tme
{
    class Network
    {
    private:
        static void* m_networkManager;

    public:
        class Engine
        {
        public:
            static TME_API ErrorCodes Init();
            static TME_API ErrorCodes ShutDown();
            static TME_API bool IsInitialized();

            static TME_API ErrorCodes EnsurInitialized();

            static TME_API ErrorCodes BeginUpdate();
            static TME_API ErrorCodes EndUpdate();
        };

        class Server
        {
        public:
            static TME_API ErrorCodes Start(
                uint16_t port
            );

            static bool TME_API IsStarted();

            static TME_API ErrorCodes GetNewClientThisTick(
                std::vector<uint32_t>& outNetworkIds
            );

            static TME_API ErrorCodes GetReceivedReliableThisTick(
                std::vector<std::pair<uint32_t, std::vector<uint8_t>>>& outMessages
            );

            static TME_API ErrorCodes SendReliableTo(
                uint32_t networkId, 
                const std::vector<uint8_t>& message
            );

            static TME_API ErrorCodes SendReliableToAll(
                const std::vector<uint8_t>& message
            );
        };

        class Client
        {
        public:
            static TME_API ErrorCodes Connect(
                const std::string& address, 
                uint16_t port
            );

            static TME_API bool IsConnected();

            static TME_API ErrorCodes GetReceivedReliableThisTick(
                std::vector<std::vector<uint8_t>>& outMessages
            );

            static TME_API ErrorCodes SendReliable(
                const std::vector<uint8_t>& message
            );
        };
    };
}