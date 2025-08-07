#include "NetworkManager.hpp"

#include "ServiceLocator.hpp"

namespace tme
{
    ErrorCodes NetworkManager::StartServer(uint16_t port)
    {
        m_serverTcpSocket = std::make_unique<TcpSocket>();

        ErrorCodes ecResult;

        ecResult = m_serverTcpSocket->Bind(port);
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("Bind failed with code: " + std::to_string(static_cast<int>(ecResult)));
            return ecResult;
        }

        ecResult = m_serverTcpSocket->Listen();
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("Listen failed with code: " + std::to_string(static_cast<int>(ecResult)));
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::StartClient(const std::string& address, uint16_t port)
    {
        m_clientTcpSocket = std::make_unique<TcpSocket>();

        ErrorCodes ecResult;

        ecResult = m_clientTcpSocket->Connect(address, port);
        if (ecResult != ErrorCodes::Success)
        {
            ServiceLocator::Logger().LogError("Connect failed with code: " + std::to_string(static_cast<int>(ecResult)));
            return ecResult;
        }

        return ErrorCodes::Success;
    }
}