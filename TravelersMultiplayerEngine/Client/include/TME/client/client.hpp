#ifndef TME_CLIENT_CLIENT_HPP
#define TME_CLIENT_CLIENT_HPP

#include "TME/export.hpp"

#include <string>
#include <cstdint>

#include "TME/errorCode.hpp"
#include "TME/engine/networkEngine.hpp"

namespace tme::client
{
	class Client
	{
	public:
		Client(Client& other) = delete;
		void operator=(const Client&) = delete;

		TME_API static Client* Get();

		TME_API ErrorCode ConnectTo(const std::string& _address, uint16_t _port);
		TME_API ErrorCode Disconnect();

	private:
		static Client* m_singleton;

		engine::NetworkEngine* m_networkEngine;

		bool m_isConnected;

		Client();
		~Client();
	};
}

#endif
