#include <iostream>
#include <thread>
#include <chrono>

#include "TME/NetworkEngine.hpp"

int main()
{
    tme::ErrorCodes ecResult;

    ecResult = tme::NetworkEngine::Init();
    if (ecResult == tme::ErrorCodes::Success)
    {
        std::cout << "TME started successfully" << std::endl;
    }
    else
    {
        std::cout << "TME failed to start" << std::endl;
    }

    ecResult = tme::NetworkEngine::StartClient("127.0.0.1", 2025);
    if (ecResult == tme::ErrorCodes::Success)
    {
        std::cout << "TME client started successfully" << std::endl;
    }
    else
    {
        std::cout << "TME client failed to start" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));

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