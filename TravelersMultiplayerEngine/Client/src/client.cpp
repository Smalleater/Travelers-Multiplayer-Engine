#include "TME/client/client.hpp"

#include "TME/debugUtils.hpp"
#include "TME/core/socketUtils.hpp"

#ifdef _WIN32
#include "TME/core/wsaInitializer.hpp"
#endif

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

		ErrorCode ec = m_networkEngine->startTcpConnectToAddress(_address, _port, false);
		if (ec != ErrorCode::Success)
		{
			TME_ERROR_LOG("Client: Failed to connect to server at %s:%d. ErrorCode: %d", _address.c_str(), _port, static_cast<int>(ec));
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
			return ErrorCode::NetworkEngineNotInitialized;
		}

		ErrorCode ec = m_networkEngine->stopTcpConnect();
		if (ec != ErrorCode::Success)
		{
			TME_ERROR_LOG("Client: Failed to disconnect from server. ErrorCode: %d", static_cast<int>(ec));
			return ec;
		}

		m_isConnected = false;

		TME_INFO_LOG("Client: Disconnected successfully.");
		return ErrorCode::Success;
	}
}