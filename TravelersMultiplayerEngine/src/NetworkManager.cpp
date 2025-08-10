#include "NetworkManager.hpp"

#include "ServiceLocator.hpp"
#include "Utils.hpp"

namespace tme
{
    ErrorCodes NetworkManager::UpdateServer()
    {
        std::unique_ptr<ISocket> newClient = m_serverTcpSocket->Accept();
        if (newClient)
        {
            std::unique_ptr<TcpSocket> tcpClient(dynamic_cast<TcpSocket*>(newClient.release()));
            if (tcpClient)
            {
                uint32_t clientId = m_nextClientId++;
                m_clients[clientId] = std::move(tcpClient);
                ServiceLocator::Logger().LogInfo("New client connected, id = " + std::to_string(clientId));
            }
            else
            {
                ServiceLocator::Logger().LogError("Accept: failed to cast ISocket* to TcpSocket*");
            }
        }

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::UpdateClient()
    {
        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::StartServer(uint16_t port)
    {
        ErrorCodes ecResult;

        #ifdef _WIN32
            if (m_wsa == nullptr)
            {
                m_wsa = std::make_unique<WsaInitializer>();

                ecResult = m_wsa->Init();
                if (ecResult != ErrorCodes::Success)
                {
                    ServiceLocator::Logger().LogError("WSA initialization failed with code: " 
                        + std::to_string(static_cast<int>(ecResult)));
                }
            }
        #endif

        m_serverTcpSocket = std::make_unique<TcpSocket>();

        ecResult = m_serverTcpSocket->Bind(port);
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("Bind failed with code: " + std::to_string(static_cast<int>(ecResult)) 
                + " Last socket error: " + std::to_string(Utils::GetLastSocketError()));
            return ecResult;
        }

        ecResult = m_serverTcpSocket->Listen();
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("Listen failed with code: " + std::to_string(static_cast<int>(ecResult))
                + " Last socket error: " + std::to_string(Utils::GetLastSocketError()));
            return ecResult;
        }

        ecResult = m_serverTcpSocket->SetBlocking(false);
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("SetBlocking failed with code: " + std::to_string(static_cast<int>(ecResult))
                + " Last socket error: " + std::to_string(Utils::GetLastSocketError()));
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::StartClient(const std::string& address, uint16_t port)
    {
        ErrorCodes ecResult;

        #ifdef _WIN32
            if (m_wsa == nullptr)
            {
                m_wsa = std::make_unique<WsaInitializer>();

                ecResult = m_wsa->Init();
                if (ecResult != ErrorCodes::Success)
                {
                    ServiceLocator::Logger().LogError("WSA initialization failed with code: " 
                        + std::to_string(static_cast<int>(ecResult)));
                }
            }
        #endif

        m_clientTcpSocket = std::make_unique<TcpSocket>();

        ecResult = m_clientTcpSocket->Connect(address, port);
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("Connect failed with code: " + std::to_string(static_cast<int>(ecResult))
                + " Last socket error: " + std::to_string(Utils::GetLastSocketError()));
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::Update()
    {
        ErrorCodes ecResult;

        if (m_serverTcpSocket != nullptr)
        {
            ecResult = UpdateServer();
            if (ecResult != ErrorCodes::Success)
            {
                return ecResult;
            }
        }

        if (m_clientTcpSocket != nullptr)
        {
            ecResult = UpdateClient();
            if (ecResult != ErrorCodes::Success)
            {
                return ecResult;
            }
        }

        return ErrorCodes::Success;
    }
}