#include <iostream>

#include "TME/NetworkEngine.hpp"

int main ()
{
    tme::ErrorCodes ecResult;

    ecResult = tme::NetworkEngine::Init();
    if (ecResult != tme::ErrorCodes::Success)
    {
        std::cout << "TME failed to start" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        return 1;
    }

    std::cout << "TME started successfully" << std::endl;

    ecResult = tme::NetworkEngine::StartServer(2025);
    if (ecResult != tme::ErrorCodes::Success)
    {
        std::cout << "TME server failed to start" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        return 1;
    }
    
    std::cout << "TME server started successfully" << std::endl;

    while (tme::NetworkEngine::IsInitialized())
    {
        tme::NetworkEngine::Update();

        std::vector<std::pair<uint32_t, std::vector<uint8_t>>> outMessages;
        tme::NetworkEngine::ReceiveAllReliableFromServer(outMessages);

        if (outMessages.size() > 0)
        {
            for (const std::pair<uint32_t, std::vector<uint8_t>>& outMessagePair : outMessages)
            {
                std::string message = std::string(outMessagePair.second.begin(), outMessagePair.second.end());
                std::cout << "New message -> clientId: " << outMessagePair.first << " message: " << message << std::endl;
            }
        }
    }
}