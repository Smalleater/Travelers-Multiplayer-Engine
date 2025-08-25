#include "NetworkManager.hpp"

#include "ServiceLocator.hpp"
#include "Utils.hpp"

namespace tme
{
    const std::vector<std::vector<uint8_t>>& NetworkManager::GetClientReceivedTcpThisTick() const
    {
        return m_clientReceivedTcpThisTick;
    }

    const std::unique_ptr<ServerCore>& NetworkManager::GetServer() const
    {
        return m_server;
    }

    bool NetworkManager::HasServer() const
    {
        return m_server != nullptr;
    }

    bool NetworkManager::HasClientSocket() const
    {
        return m_clientTcpSocket != nullptr;
    }

    void NetworkManager::AddMessageToClientTcpSendQueue(const std::vector<uint8_t>& data)
    {
        m_clientTcpSendQueue.push_back(data);
    }

    ErrorCodes NetworkManager::StartServer(uint16_t port)
    {
        if (m_server != nullptr)
        {
            if (m_server->IsStarted())
            {
                ServiceLocator::Logger().LogWarning("StartServer called but server is already started");
                return ErrorCodes::AlreadyStarted;
            }
        }

        ErrorCodes ecResult;
        int lastSocketError;

        #ifdef _WIN32
            if (m_wsa == nullptr)
            {
                m_wsa = std::make_unique<WsaInitializer>();
                
                ecResult = m_wsa->Init();
                lastSocketError = m_wsa->GetWsaStartupResult();
                if (ecResult != ErrorCodes::Success)
                {
                    Utils::LogSocketError("StartServer : WSA initialization", ecResult, lastSocketError);
                    return ecResult;
                }
            }
        #endif

        if (m_server == nullptr)
        {
            m_server = std::make_unique<ServerCore>();
        }

        ecResult = m_server->Start(port);
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::StartClient(const std::string& address, uint16_t port)
    {
        ErrorCodes ecResult;
        int lastSocketError;

        if (m_clientTcpSocket != nullptr)
        {
            ServiceLocator::Logger().LogWarning("StartClient called but client socket already exists");
            return ErrorCodes::AlreadyInitialized;
        }

        #ifdef _WIN32
            if (m_wsa == nullptr)
            {
                m_wsa = std::make_unique<WsaInitializer>();

                ecResult = m_wsa->Init();
                lastSocketError = m_wsa->GetWsaStartupResult();
                if (ecResult != ErrorCodes::Success)
                {
                    Utils::LogSocketError("StartClient: WSA initialization", ecResult, lastSocketError);
                    return ecResult;
                }
            }
        #endif

        m_clientTcpSocket = std::make_unique<TcpSocket>();

        ecResult = m_clientTcpSocket->Connect(address, port);
        lastSocketError = m_clientTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("StartClient: Connect", ecResult, lastSocketError);
            return ecResult;
        }

        ecResult = m_clientTcpSocket->SetBlocking(false);
        lastSocketError = m_clientTcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("StartClient: SetBlocking", ecResult, lastSocketError);
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::BeginUpdate()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        if (m_server != nullptr)
        {
            ecResult = m_server->BeginUpdate();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        if (m_clientTcpSocket != nullptr)
        {
            ecResult = BeginUpdateClient();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::EndUpdate()
    {
        bool hadSuccess = false;
        bool hadError = false;
        
        ErrorCodes ecResult;

        if (m_server != nullptr)
        {
            ecResult = m_server->EndUpdate();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        if (m_clientTcpSocket != nullptr)
        {
            ecResult = EndUpdateClient();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::BeginUpdateClient()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        ecResult = ClientReceivedTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::EndUpdateClient()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        ecResult = ClientSendTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::ClientReceivedTcp()
    {
        m_clientReceivedTcpThisTick.clear();

        const uint8_t maxMessagesPerFrame = 32;
        uint8_t messagesReceivedThisFrame = 0;

        int bytesReceived;
        std::vector<uint8_t> buffer(4096);

        ErrorCodes ecResult;
        int lastSocketError;

        while (messagesReceivedThisFrame < maxMessagesPerFrame)
        {
            bytesReceived = 0;
            buffer.resize(4096);

            ecResult = m_clientTcpSocket->Receive(buffer.data(), buffer.size(), bytesReceived);
            lastSocketError = m_clientTcpSocket->GetLastSocketError();
            if (ecResult != ErrorCodes::Success)
            {
                if (ecResult == ErrorCodes::ReceiveConnectionClosed)
                {
                    m_clientTcpSocket->Shutdown();
                    m_clientTcpSocket.reset();
                    ServiceLocator::Logger().LogInfo(
                        "Disconnected from server (connection closed by remote host)");
                    return ErrorCodes::ReceiveConnectionClosed;
                }
                else if (ecResult != ErrorCodes::ReceiveWouldBlock)
                {
                    Utils::LogSocketError("ReceiveFromClientTcp: Receive", ecResult, lastSocketError);
                    
                    m_clientTcpSocket->Shutdown();
                    m_clientTcpSocket.reset();
                    ServiceLocator::Logger().LogInfo(
                        "Disconnected from server (connection closed by remote host)");
                    
                    return messagesReceivedThisFrame > 0 
                        ? ErrorCodes::PartialSuccess : ErrorCodes::ReceiveFailed;
                }

                break;
            }

            buffer.resize(bytesReceived);
            m_clientReceivedTcpThisTick.emplace_back(std::move(buffer));

            messagesReceivedThisFrame++;
        }
        
        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::ClientSendTcp()
    {
        ErrorCodes ecResult;
        int lastSocketError;

        int bytesSent;

        for (const std::vector<uint8_t>& message : m_clientTcpSendQueue)
        {
            ecResult = m_clientTcpSocket->Send(message.data(), message.size(), bytesSent);
            lastSocketError = m_clientTcpSocket->GetLastSocketError();
            if (ecResult == ErrorCodes::SendConnectionClosed)
            {
                m_clientTcpSocket->Shutdown();
                m_clientTcpSocket.reset();
                ServiceLocator::Logger().LogInfo(
                    "Disconnected from server (connection closed by remote host)");

                m_clientTcpSendQueue.clear();
                return ErrorCodes::ReceiveConnectionClosed;
            }
            else if (ecResult != ErrorCodes::Success)
            {
                Utils::LogSocketError("SendToServerTcp: Send", ecResult, lastSocketError);

                m_clientTcpSocket->Shutdown();
                m_clientTcpSocket.reset();
                ServiceLocator::Logger().LogInfo(
                    "Disconnected from server (connection closed by remote host)");

                m_clientTcpSendQueue.clear();
                return ecResult;
            }
        }

        m_clientTcpSendQueue.clear();
        return ErrorCodes::Success;
    }
}