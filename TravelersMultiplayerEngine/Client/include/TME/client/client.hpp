#ifndef TME_CLIENT_CLIENT_HPP
#define TME_CLIENT_CLIENT_HPP

#include "TME/export.hpp"

#include <string>
#include <cstdint>

#include "TME/errorCode.hpp"
#include "TME/engine/networkEngine.hpp"

namespace tme::client
{
	using EntityId = uint64_t;

	class Client
	{
	public:
		Client(Client& other) = delete;
		void operator=(const Client&) = delete;

		TME_API static Client* Get();

		TME_API ErrorCode ConnectTo(const std::string& _address, uint16_t _port);
		TME_API ErrorCode Disconnect();

		TME_API bool IsConnected() const;

		TME_API void beginUpdate();
		TME_API void endUpdate();

		TME_API ErrorCode sendTcpMessage(std::shared_ptr<engine::Message> _message);

		template<typename ...ComponentType>
		std::vector<EntityId> queryIds()
		{
			return m_networkEngine->queryIds<ComponentType...>();
		}

		template<typename ...ComponentType>
		std::vector<std::tuple<EntityId, std::shared_ptr<ComponentType>...>> query()
		{
			return m_networkEngine->query<ComponentType...>();
		}

	private:
		static Client* m_singleton;

		engine::NetworkEngine* m_networkEngine;

		bool m_isConnected;

		Client();
		~Client();
	};
}

#endif
