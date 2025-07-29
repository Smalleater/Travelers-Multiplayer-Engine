#include <iostream>
#include <thread>
#include <chrono>

#include "TME/NetworkEngine.hpp"

int main()
{
    std::cout << "Hello world" << std::endl;

    tme::ErrorCodes result;

    result = tme::NetworkEngine::Init();
    if (result == tme::ErrorCodes::Success)
    {
        std::cout << "TME started successfully" << std::endl;
    }
    else
    {
        std::cout << "TME started with error" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    result = tme::NetworkEngine::ShutDown();
    if (result == tme::ErrorCodes::Success)
    {
        std::cout << "TME shutdown successfully" << std::endl;
    }
    else if (result == tme::ErrorCodes::CompletedWithErrors)
    {
        std::cout << "TME shutdown completed with errors" << std::endl;
    }
    else
    {
        std::cout << "TME shutdown failur" << std::endl;
    }

    system("pause");
}