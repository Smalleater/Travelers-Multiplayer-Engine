#include "Core/ClientCore.hpp"

#include "ServiceLocator.hpp"
#include "Utils.hpp"
#include "TcpMessage.hpp"

namespace tme
{
    ClientCore::~ClientCore()
    {
        Disconnect();
    }

    ErrorCodes ClientCore::ConnectTo(const std::string& address, uint16_t port)
    {
        if (IsConnected())
        {
            ServiceLocator::Logger().LogWarning("ClientCore::ConnectTo called but client is already connected");
            return ErrorCodes::ClientAlreadyConnected;
        }

        ErrorCodes ecResult;
        int lastSocketError;

        m_tcpSocket = std::make_unique<TcpSocket>();

        ecResult = m_tcpSocket->Connect(address, port);
        lastSocketError = m_tcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("ClientCore::ConnectTo: Connect", ecResult, lastSocketError);
            return ecResult;
        }

        m_messageIdGenerator = std::make_unique<MessageIdGenerator>();

        ecResult = m_tcpSocket->SetBlocking(false);
        lastSocketError = m_tcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("ClientCore::ConnectTo: SetBlocking", ecResult, lastSocketError);
            return ecResult;
        }

        return ErrorCodes::Success;
    }

    ErrorCodes ClientCore::Disconnect()
    {
        if (!IsConnected())
        {
            return ErrorCodes::Success;
        }

        ErrorCodes ecResult = m_tcpSocket->Shutdown();
        int lastSocketError = m_tcpSocket->GetLastSocketError();
        if (ecResult != ErrorCodes::Success)
        {
            Utils::LogSocketError("ClientCore::Disconnect: Shutdown client socket", ecResult, lastSocketError);
        }

        m_tcpSocket.reset();
        m_messageIdGenerator.reset();

        m_receiveBuffer.clear();
        m_receivedTcpThisTick.clear();
        m_tcpSendQueue.clear();

        return ecResult;
    }

    ErrorCodes ClientCore::BeginUpdate()
    {
        if (!IsConnected())
        {
            ServiceLocator::Logger().LogWarning("ClientCore::BeginUpdate called but client are not connected");
            return ErrorCodes::ClientNotConnected;
        }

        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        ecResult = ReceivedTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    ErrorCodes ClientCore::EndUpdate()
    {
        if (!IsConnected())
        {
            ServiceLocator::Logger().LogWarning("ClientCore::EndUpdate called but client are not connected");
            return ErrorCodes::ClientNotConnected;
        }

        bool hadSuccess = false;
        bool hadError = false;

        ErrorCodes ecResult;

        ecResult = SendTcp();
        Utils::UpdateSuccessErrorFlags(ecResult, hadSuccess, hadError);

        return Utils::GetCombinedErrorCode(hadSuccess, hadError);
    }

    bool ClientCore::IsConnected()
    {
        return m_tcpSocket != nullptr;
    }

    const std::vector<std::vector<uint8_t>>& ClientCore::GetReceivedTcpThisTick() const
    {
        return m_receivedTcpThisTick;
    }

    void ClientCore::AddMessageToTcpSendQueue(const std::vector<uint8_t>& data)
    {
        m_tcpSendQueue.push_back(data);
    }

    ErrorCodes ClientCore::ReceivedTcp()
    {
        m_receivedTcpThisTick.clear();

        const uint8_t maxMessagesPerFrame = 32;
        uint8_t messagesReceivedThisFrame = 0;

        int bytesReceived;
        std::vector<uint8_t> tempBuffer(4096);

        ErrorCodes ecResult;
        int lastSocketError;

        while (messagesReceivedThisFrame < maxMessagesPerFrame)
        {
            bytesReceived = 0;
            tempBuffer.resize(4096);

            ecResult = m_tcpSocket->Receive(tempBuffer.data(), tempBuffer.size(), bytesReceived);
            lastSocketError = m_tcpSocket->GetLastSocketError();
            if (ecResult != ErrorCodes::Success)
            {
                if (ecResult == ErrorCodes::ReceiveConnectionClosed)
                {
                    Disconnect();
                    ServiceLocator::Logger().LogInfo("Disconnected from server (connection closed by remote host)");

                    return ErrorCodes::ReceiveConnectionClosed;
                }
                else if (ecResult != ErrorCodes::ReceiveWouldBlock)
                {
                    Utils::LogSocketError("ReceiveFromClientTcp: Receive", ecResult, lastSocketError);

                    Disconnect();
                    ServiceLocator::Logger().LogInfo(
                        "Disconnected from server (connection closed by remote host)");

                    return messagesReceivedThisFrame > 0 
                        ? ErrorCodes::PartialSuccess : ErrorCodes::ReceiveFailed;
                }

                break;
            }

            tempBuffer.resize(bytesReceived);
            m_receiveBuffer.insert(m_receiveBuffer.end(), tempBuffer.begin(), tempBuffer.end());
            
            while (TcpMessage::HasCompleteHeader(m_receiveBuffer) && messagesReceivedThisFrame < maxMessagesPerFrame)
            {
                uint32_t payloadSize = TcpMessage::GetPayloadSizeFromHeader(m_receiveBuffer);
                uint32_t messageSize = TcpMessage::HEADER_SIZE + payloadSize;

                if (!TcpMessage::HasCompleteMessage(m_receiveBuffer))
                {
                    break;
                }

                TcpMessage message(MessageType::UNKNOWN);
                if (message.Deserialize(m_receiveBuffer.data(), messageSize))
                {
                    std::vector<uint8_t> payload = message.GetPayload();
                    m_receivedTcpThisTick.emplace_back(std::move(payload));

                    messagesReceivedThisFrame++;
                }
                else
                {
                    ServiceLocator::Logger().LogError("ClientCore::ReceivedTcp: Failed to deserialize TCP message");
                    m_receiveBuffer.clear();
                    break;
                }

                m_receiveBuffer.erase(m_receiveBuffer.begin(), m_receiveBuffer.begin() + messageSize);
            }

            if (m_receiveBuffer.size() > 64 * 1024)
            {
                ServiceLocator::Logger().LogWarning("ClientCore::ReceivedTcp: Receive buffer to large, clearing");
                m_receiveBuffer.clear();
                break;
            }
        }
        
        return ErrorCodes::Success;
    }

    ErrorCodes ClientCore::SendTcp()
    {
        ErrorCodes ecResult;
        int lastSocketError;

        int bytesSent;
        
        for (const std::vector<uint8_t>& data : m_tcpSendQueue)
        {
            TcpMessage message(MessageType::GAME_DATA, m_messageIdGenerator->GetUniqueId());
            message.SetPayload(data);

            std::vector<uint8_t> serializedMessage = message.Serialize();

            ecResult = m_tcpSocket->Send(serializedMessage.data(), serializedMessage.size(), bytesSent);
            lastSocketError = m_tcpSocket->GetLastSocketError();
            if (ecResult == ErrorCodes::SendConnectionClosed)
            {
                Disconnect();
                ServiceLocator::Logger().LogInfo(
                    "Disconnected from server (connection closed by remote host)");

                return ErrorCodes::ReceiveConnectionClosed;
            }
            else if (ecResult != ErrorCodes::Success)
            {
                Utils::LogSocketError("SendToServerTcp: Send", ecResult, lastSocketError);

                Disconnect();
                ServiceLocator::Logger().LogInfo(
                    "Disconnected from server (connection closed by remote host)");

                return ecResult;
            }
        }

        m_tcpSendQueue.clear();
        return ErrorCodes::Success;
    }
}