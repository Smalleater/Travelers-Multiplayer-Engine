#include <iostream>
#include <thread>
#include <chrono>

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

    while (tme::NetworkEngine::IsServerStarted())
    {
        tme::NetworkEngine::Update();

        std::string messageToSend("Hello world from server");
        std::vector<uint8_t> serializedMessage(messageToSend.begin(), messageToSend.end());

        ecResult = tme::NetworkEngine::SendToAllClientReliable(serializedMessage);
        if (ecResult != tme::ErrorCodes::Success)
        {
            std::cout << "Failed to send message" << std::endl;
        }

        std::vector<std::pair<uint32_t, std::vector<uint8_t>>> outMessages;

        ecResult = tme::NetworkEngine::ReceiveAllReliableFromClient(outMessages);
        if (ecResult != tme::ErrorCodes::Success)
        {
            std::cout << "Receive failed" << std::endl;
        }
        else if (outMessages.size() > 0)
        {
            for (const std::pair<uint32_t, std::vector<uint8_t>>& outMessagePair : outMessages)
            {
                std::string message = std::string(outMessagePair.second.begin(), outMessagePair.second.end());
                std::cout << "New message -> clientId: " << outMessagePair.first << " message: " << message << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ecResult = tme::NetworkEngine::ShutDown();
    if (ecResult == tme::ErrorCodes::Success)
    {
        std::cout << "TME shutdown successfully" << std::endl;
    }
    else if (ecResult == tme::ErrorCodes::CompletedWithErrors)
    {
        std::cout << "TME shutdown completed, but with errors" << std::endl;
    }
    else
    {
        std::cout << "TME failed to shutdown" << std::endl;
    }

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
}