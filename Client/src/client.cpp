#include "TRA/client/client.hpp"

#include "TRA/debugUtils.hpp"
#include "TRA/engine/connectionStatusComponent.hpp"

namespace tra::client
{
	Client* Client::m_singleton = nullptr;

	Client::Client()
	{
		m_networkEngine = new engine::NetworkEngine;
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
		TRA_ASSERT_REF_PTR_OR_COPIABLE(_address);

		if (IsConnected())
		{
			TRA_DEBUG_LOG("Client: ConnectTo called but client is already connected.");
			return ErrorCode::ClientAlreadyConnected;
		}

		if (!m_networkEngine)
		{
			TRA_ERROR_LOG("Client: Network engine is not initialized.");
			return ErrorCode::NetworkEngineNotInitialized;
		}

		ErrorCode ec;

		ec = m_networkEngine->startTcpConnectToAddress(_address, _port, false);
		if (ec != ErrorCode::Success)
		{
			return ec;
		}

		/*ec = m_networkEngine->startUdpOnPort(0, false);
		if (ec != ErrorCode::Success)
		{
			m_networkEngine->stopTcpConnect();
			return ec;
		}*/

		TRA_INFO_LOG("Client: Successfully connected to server at %s:%d.", _address.c_str(), _port);
		return ErrorCode::Success;
	}

	ErrorCode Client::Disconnect()
	{
		if (!IsConnected())
		{
			TRA_DEBUG_LOG("Client: Disconnect called but client is not connected.");
			return ErrorCode::Success;
		}

		if (!m_networkEngine)
		{
			TRA_ERROR_LOG("Client: Network engine is not initialized.");
			return ErrorCode::NetworkEngineNotInitialized;
		}

		ErrorCode ecTcp = m_networkEngine->stopTcpConnect();
		//ErrorCode ecUdp = m_networkEngine->stopUdp();

		if (ecTcp != ErrorCode::Success /*|| ecUdp != ErrorCode::Success*/)
		{
			TRA_ERROR_LOG("Client: Disconnection encountered errors. TCP ErrorCode: %d", static_cast<int>(ecTcp));
			return ErrorCode::DisconnectWithErrors;
		}
		else
		{
			TRA_INFO_LOG("Client: Disconnected successfully.");
			return ErrorCode::Success;
		}
	}

	bool Client::IsConnected() const
	{
		return m_networkEngine->entityHasComponent<engine::ConnectedComponentTag>(m_networkEngine->getSelfEntityId());
	}

	void Client::beginUpdate()
	{
		if (!IsConnected())
		{
			return;
		}

		m_networkEngine->beginUpdate();
	}

	void Client::endUpdate()
	{
		if (!IsConnected())
		{
			return;
		}

		m_networkEngine->endUpdate();
	}

	ErrorCode Client::sendTcpMessage(std::shared_ptr<engine::Message> _message)
	{
		if (!IsConnected())
		{
			return ErrorCode::ClientNotConnected;
		}

		return m_networkEngine->sendTcpMessage(m_networkEngine->getSelfEntityId(), _message);
	}

	std::vector<std::shared_ptr<engine::Message>> Client::getTcpMessages(const std::string& _messageType)
	{
		if (!IsConnected())
		{
			return {};
		}

		return m_networkEngine->getTcpMessages(m_networkEngine->getSelfEntityId(), _messageType);
	}
}