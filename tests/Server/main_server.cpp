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
    }
}