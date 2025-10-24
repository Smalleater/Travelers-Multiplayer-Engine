#ifndef TME_SERVER_SERVER_HPP
#define TME_SERVER_SERVER_HPP

#include "TME/export.hpp"

#include <cstdint>

namespace tme::server
{
	class Server
	{
	public:
		Server(Server& other) = delete;
		void operator=(const Server&) = delete;

		TME_API static Server* Get();

		TME_API void Start(uint16_t _port);
		TME_API void Stop();

	private:
		static Server m_singleton;

		Server();
		~Server();
	};

	
}

#endif