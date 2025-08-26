#include "NetworkManager.hpp"

#include "ServiceLocator.hpp"
#include "Utils.hpp"

namespace tme
{
    ErrorCodes NetworkManager::StartServer(uint16_t port)
    {
        if (m_server != nullptr)
        {
            if (m_server->IsStarted())
            {
                ServiceLocator::Logger().LogWarning("NetworkManager::StartServer called but server is already started");
                return ErrorCodes::ServerAlreadyStarted;
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
                    Utils::LogSocketError("NetworkManager::StartServer : WSA initialization", ecResult, lastSocketError);
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

    ErrorCodes NetworkManager::ConnectClient(const std::string& address, uint16_t port)
    {
        if (m_client != nullptr)
        {
            if (m_client->IsConnected())
            {
                ServiceLocator::Logger().LogWarning("NetworkManager::StartClient called but client is already connected");
                return ErrorCodes::ClientAlreadyConnected;
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
                    Utils::LogSocketError("StartClient: WSA initialization", ecResult, lastSocketError);
                    return ecResult;
                }
            }
        #endif

        if (m_client == nullptr)
        {
            m_client = std::make_unique<ClientCore>();
        }

        ecResult = m_client->ConnectTo(address, port);
        if (ecResult != ErrorCodes::Success)
        {
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes NetworkManager::BeginUpdate()
    {
        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        if (m_server != nullptr && m_server->IsStarted())
        {
            ecResult = m_server->BeginUpdate();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        if (m_client != nullptr && m_client->IsConnected())
        {
            ecResult = m_client->BeginUpdate();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes NetworkManager::EndUpdate()
    {
        bool hadSuccess = false;
        bool hadError = false;
        
        ErrorCodes ecResult;

        if (m_server != nullptr && m_server->IsStarted())
        {
            ecResult = m_server->EndUpdate();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        if (m_client != nullptr && m_client->IsConnected())
        {
            ecResult = m_client->EndUpdate();
            Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);
        }

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    const std::unique_ptr<ServerCore>& NetworkManager::GetServer() const
    {
        return m_server;
    }

    const std::unique_ptr<ClientCore>& NetworkManager::GetClient() const
    {
        return m_client;
    }

    bool NetworkManager::HasServer() const
    {
        return m_server != nullptr;
    }

    bool NetworkManager::HasClient() const
    {
        return m_client != nullptr;
    }
}