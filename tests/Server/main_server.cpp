#include <iostream>
#include <thread>
#include <chrono>

#include "TME/NetworkEngine.hpp"

int main ()
{
    tme::ErrorCodes ecResult;

    ecResult = tme::Network::Engine::Init();
    if (ecResult != tme::ErrorCodes::Success)
    {
        std::cout << "TME failed to start" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        return 1;
    }

    std::cout << "TME started successfully" << std::endl;

    ecResult = tme::Network::Server::Start(2025);
    if (ecResult != tme::ErrorCodes::Success)
    {
        std::cout << "TME server failed to start" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        return 1;
    }
    
    std::cout << "TME server started successfully" << std::endl;

    //int test = 0;
    while (tme::Network::Server::IsStarted())
    {
        ecResult = tme::Network::Engine::BeginUpdate();
        if (ecResult != tme::ErrorCodes::Success)
        {
            std::cout << "BeginUpdate failed with code: " << std::to_string(static_cast<int>(ecResult)) << std::endl;
        }

        std::vector<uint32_t> newClients;
        ecResult = tme::Network::Server::GetNewClientThisTick(newClients);
        if (ecResult != tme::ErrorCodes::Success)
        {
            std::cout << "GetNewClientThisTick failed with code: " << std::to_string(static_cast<int>(ecResult)) << std::endl;
        }

        for (uint32_t networkId : newClients)
        {
            std::cout << "New client connected with network id: " << networkId << std::endl;
        }

        std::vector<std::pair<uint32_t, std::vector<uint8_t>>> outMessages;

        ecResult = tme::Network::Server::GetReceivedReliableThisTick(outMessages);
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

        std::string messageToSend("Hello world from server");
        std::vector<uint8_t> serializedMessage(messageToSend.begin(), messageToSend.end());

        ecResult = tme::Network::Server::SendReliableToAll(serializedMessage);
        if (ecResult != tme::ErrorCodes::Success)
        {
            std::cout << "Failed to send message" << std::endl;
        }

        ecResult = tme::Network::Engine::EndUpdate();
        if (ecResult != tme::ErrorCodes::Success)
        {
            std::cout << "EndUpdate failed with code: " << std::to_string(static_cast<int>(ecResult)) << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        //test++;
    }

    ecResult = tme::Network::Engine::ShutDown();
    if (ecResult == tme::ErrorCodes::Success)
    {
        std::cout << "TME shutdown successfully" << std::endl;
    }
    else if (ecResult == tme::ErrorCodes::PartialSuccess)
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