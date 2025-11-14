#include "TME/client/client.hpp"

#include "TME/debugUtils.hpp"

namespace tme::client
{
	Client* Client::m_singleton = nullptr;

	Client::Client()
	{
		m_networkEngine = new engine::NetworkEngine;
		m_isConnected = false;
	}

	Client::~Client()
	{
		Disconnect();
	}

	Client* Client::Get()
	{
		if (m_singleton == nullptr)
		{
			m_singleton = new Client();
		}
		return m_singleton;
	}

	ErrorCode Client::ConnectTo(const std::string& _address, uint16_t _port)
	{
		TME_ASSERT_REF_PTR_OR_COPIABLE(_address);

		if (m_isConnected)
		{
			TME_DEBUG_LOG("Client: ConnectTo called but client is already connected.");
			return ErrorCode::ClientAlreadyConnected;
		}

		if (!m_networkEngine)
		{
			TME_ERROR_LOG("Client: Network engine is not initialized.");
			return ErrorCode::NetworkEngineNotInitialized;
		}

		ErrorCode ec;

		ec = m_networkEngine->startTcpConnectToAddress(_address, _port, false);
		if (ec != ErrorCode::Success)
		{
			return ec;
		}

		ec = m_networkEngine->startUdpOnPort(0, false);
		if (ec != ErrorCode::Success)
		{
			m_networkEngine->stopTcpConnect();
			return ec;
		}

		m_isConnected = true;

		TME_INFO_LOG("Client: Successfully connected to server at %s:%d.", _address.c_str(), _port);
		return ErrorCode::Success;
	}

	ErrorCode Client::Disconnect()
	{
		if (!m_isConnected)
		{
			TME_DEBUG_LOG("Client: Disconnect called but client is not connected.");
			return ErrorCode::Success;
		}

		if (!m_networkEngine)
		{
			TME_ERROR_LOG("Client: Network engine is not initialized.");
			m_isConnected = false;
			return ErrorCode::NetworkEngineNotInitialized;
		}

		ErrorCode ecTcp = m_networkEngine->stopTcpConnect();
		ErrorCode ecUdp = m_networkEngine->stopUdp();

		m_isConnected = false;

		if (ecTcp != ErrorCode::Success || ecUdp != ErrorCode::Success)
		{
			TME_ERROR_LOG("Client: Disconnection encountered errors. TCP ErrorCode: %d, UDP ErrorCode: %d", static_cast<int>(ecTcp), static_cast<int>(ecUdp));
			return ErrorCode::DisconnectWithErrors;
		}
		else
		{
			TME_INFO_LOG("Client: Disconnected successfully.");
			return ErrorCode::Success;
		}
	}

	bool Client::IsConnected() const
	{
		return m_isConnected;
	}

	void Client::beginUpdate()
	{
		if (!m_isConnected)
		{
			TME_ERROR_LOG("Client: Cannot begin update, client is not connected.");
			return;
		}

		m_networkEngine->beginUpdate();
	}

	void Client::endUpdate()
	{
		if (!m_isConnected)
		{
			TME_ERROR_LOG("Client: Cannot end update, client is not connected.");
			return;
		}

		m_networkEngine->endUpdate();
	}

	ErrorCode Client::sendTcpMessage(std::shared_ptr<engine::Message> _message)
	{
		if (!m_isConnected)
		{
			TME_ERROR_LOG("Client: Cannot send TCP message, client is not connected.");
			return ErrorCode::ClientNotConnected;
		}

		return m_networkEngine->sendTcpMessage(m_networkEngine->getSelfEntityId(), _message);
	}
}