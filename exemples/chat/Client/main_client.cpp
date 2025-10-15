#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <Windows.h>
#include <conio.h>

#include "TME/NetworkEngine.hpp"

std::atomic<bool> running(true);
std::atomic<bool> sentMessage(false);
std::mutex coutMutex;
std::string userInput;
std::string sentBuffer;

std::vector<std::string> messageBuffer;
const int maxMessages = 10;

void setCursorPosition(short x, short y)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { x, y };
    SetConsoleCursorPosition(hConsole, pos);
}

void RefreshConsole()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    short baseY = csbi.srWindow.Bottom - maxMessages - 1;

    for (int i = 0; i < maxMessages; i++)
    {
        setCursorPosition(0, baseY + i);
        std::cout << std::string(100, ' ');
        setCursorPosition(0, baseY + i);
        if (i < messageBuffer.size())
        {
            std::cout << messageBuffer[i];
        }
    }
            
    setCursorPosition(0, csbi.srWindow.Bottom);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    std::string prompt = "> " + userInput;

    if (prompt.size() < static_cast<size_t>(consoleWidth))
    {
        prompt += std::string(consoleWidth - prompt.size(), ' ');
    }
    else
    {
        prompt = prompt.substr(0, consoleWidth);
    }

    std::cout << prompt;
    setCursorPosition(2 + userInput.size(), csbi.srWindow.Bottom);
    std::cout.flush();
}

void inputThreadFunc()
{
    std::string buffer;
    while (running)
    {
        if (_kbhit())
        {
            char ch = _getch();
            if (ch == '\r')
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                userInput = buffer;
                if (buffer == "/quit")
                {
                    running = false;
                }
                else
                {
                    sentBuffer = userInput;
                    sentMessage = true;
                }
                buffer.clear();
            }
            else if (ch == '\b')
            {
                if (!buffer.empty())
                {
                    buffer.pop_back();
                    std::lock_guard<std::mutex> lock(coutMutex);
                    std::cout << "\b \b";
                    std::cout.flush();
                }
            }
            else if (isprint(static_cast<unsigned char>(ch)))
            {
                buffer += ch;
            }

            {
                std::lock_guard<std::mutex> lock(coutMutex);
                userInput = buffer;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main()
{
    tme::ErrorCodes ecResult;

    ecResult = tme::Network::Engine::Init();
    if (ecResult == tme::ErrorCodes::Success)
    {
        std::cout << "TME started successfully" << std::endl;
    }
    else
    {
        std::cout << "TME failed to start" << std::endl;
    }

    ecResult = tme::Network::Client::ConnectTo("127.0.0.1", 2025);
    if (ecResult == tme::ErrorCodes::Success)
    {
        std::cout << "TME client started successfully" << std::endl;
    }
    else
    {
        std::cout << "TME client failed to start" << std::endl;
    }

    std::string lastUserInput;
    std::vector<std::string> lastMessageBuffer;

    sentBuffer = "";
    sentMessage = false;
    running = true;

    RefreshConsole();

    std::thread inputThread(inputThreadFunc);
    while (tme::Network::Client::IsConnected() && running)
    {
        ecResult = tme::Network::Engine::BeginUpdate();
        if (ecResult != tme::ErrorCodes::Success)
        {
            //std::cout << "BeginUpdate failed with code: " << std::to_string(static_cast<int>(ecResult)) << std::endl;
        }

        if (sentMessage)
        {
            std::string str = sentBuffer;
            std::vector<uint8_t> message(str.begin(), str.end());

            ecResult = tme::Network::Client::SendReliable(message);
            if (ecResult != tme::ErrorCodes::Success)
            {
                //std::cout << "failed to send message" << std::endl;
            }

            sentMessage = false;
        }

        std::vector<std::vector<uint8_t>> outMessages;

        ecResult = tme::Network::Client::GetReceivedReliableThisTick(outMessages);
        if (ecResult != tme::ErrorCodes::Success)
        {
            //std::cout << "failed to receive message" << std::endl;
        }

        bool needRefresh = false;

        {
            std::lock_guard<std::mutex> lock(coutMutex);

            if (outMessages.size() > 0)
            {
                for (std::vector<uint8_t>& outMessage : outMessages)
                {
                    std::string message(outMessage.begin(), outMessage.end());
                    messageBuffer.push_back("-> " + message);
                    if (messageBuffer.size() > maxMessages)
                    {
                        messageBuffer.erase(messageBuffer.begin());
                    }
                }
            }

            if (messageBuffer != lastMessageBuffer)
            {
                lastMessageBuffer = messageBuffer;
                needRefresh = true;
            }

            if (userInput != lastUserInput)
            {
                lastUserInput = userInput;
                needRefresh = true;
            }

            if (needRefresh)
            {
                RefreshConsole();
            }
        }

        ecResult = tme::Network::Engine::EndUpdate();
        if (ecResult != tme::ErrorCodes::Success)
        {
            std::cout << "EndUpdate failed with code: " << std::to_string(static_cast<int>(ecResult)) << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        //test++;
    }

    running = false;
    if (inputThread.joinable())
    {
        inputThread.join();
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