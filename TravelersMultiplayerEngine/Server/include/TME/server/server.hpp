#ifndef TME_SERVER_SERVER_HPP
#define TME_SERVER_SERVER_HPP

#include "TME/export.hpp"

#include <cstdint>
#include <unordered_map>

#include "TME/errorCode.hpp"
#include "TME/engine/networkEngine.hpp"

namespace tme::server
{
	using EntityId = uint64_t;

	class Server
	{
	public:
		Server(Server& other) = delete;
		void operator=(const Server&) = delete;

		TME_API static Server* Get();

		TME_API ErrorCode Start(uint16_t _port);
		TME_API ErrorCode Stop();

		TME_API bool isRunning() const;

		TME_API void beginUpdate();
		TME_API void endUpdate();

		TME_API ErrorCode sendTcpMessage(engine::EntityId _entityId, std::shared_ptr<engine::Message> _message);

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
		static Server* m_singleton;

		engine::NetworkEngine* m_networkEngine;

		bool m_isRunning;

		Server();
		~Server();
	};

	
}

#endif