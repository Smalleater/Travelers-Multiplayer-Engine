#include "Core/EngineCore.hpp"

#include "ServiceLocator.hpp"
#include "Utils.hpp"

namespace tme
{
    ErrorCodes EngineCore::StartServer(uint16_t port)
    {
        if (m_server != nullptr)
        {
            if (m_server->IsStarted())
            {
                ServiceLocator::Logger().LogWarning("EngineCore::StartServer called but server is already started");
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
                    Utils::LogSocketError("EngineCore::StartServer: WSA initialization", ecResult, lastSocketError);
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

    ErrorCodes EngineCore::StopServer()
    {
        if (m_server == nullptr)
        {
            return ErrorCodes::Success;
        }

        ErrorCodes ecResult = ErrorCodes::Success;
        if (m_server->IsStarted())
        {
            ecResult = m_server->Stop();
        }

        m_server.reset();
        
        ServiceLocator::Logger().LogInfo("EngineCore::StopServer: Server stopped with code: " 
            + std::to_string(static_cast<int>(ecResult)));

        return ecResult == ErrorCodes::Success ? ErrorCodes::Success : ErrorCodes::PartialSuccess;
    }

    ErrorCodes EngineCore::ConnectClient(const std::string& address, uint16_t port)
    {
        if (m_client != nullptr)
        {
            if (m_client->IsConnected())
            {
                ServiceLocator::Logger().LogWarning("EngineCore::StartClient: called but client is already connected");
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
                    Utils::LogSocketError("EngineCore::StartClient: WSA initialization", ecResult, lastSocketError);
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

    ErrorCodes EngineCore::DisconnectClient()
    {
        if (m_client == nullptr)
        {
            return ErrorCodes::Success;
        }

        ErrorCodes ecResult = ErrorCodes::Success;
        if (m_client->IsConnected())
        {
            ecResult = m_client->Disconnect();
        }

        m_client.reset();

        ServiceLocator::Logger().LogInfo("EngineCore::DisconnectClient: client disconnected with code: " 
            + std::to_string(static_cast<int>(ecResult)));

        return ecResult == ErrorCodes::Success ? ErrorCodes::Success : ErrorCodes::PartialSuccess;
    }

    ErrorCodes EngineCore::BeginUpdate()
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

    ErrorCodes EngineCore::EndUpdate()
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

    const std::unique_ptr<ServerCore>& EngineCore::GetServer() const
    {
        return m_server;
    }

    const std::unique_ptr<ClientCore>& EngineCore::GetClient() const
    {
        return m_client;
    }

    bool EngineCore::IsServerStarted() const
    {
        if (m_server == nullptr)
        {
            return false;
        }

        return m_server->IsStarted();
    }

    bool EngineCore::IsClientConnected() const
    {
        if (m_client == nullptr)
        {
            return false;
        }

        return m_client->IsConnected();
    }
}