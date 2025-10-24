#ifndef TME_CORE_WSA_INITIALIZER_HPP
#define TME_CORE_WSA_INITIALIZER_HPP

#ifdef _WIN32

#include "TME/export.hpp"
#include "TME/errorCode.hpp"

#include <WinSock2.h>

namespace tme::Core
{
	class WSAInitializer
	{
	public:
		WSAInitializer(WSAInitializer& other) = delete;
		void operator=(const WSAInitializer&) = delete;

		static WSAInitializer* Get();

		ErrorCode Init();
		void CleanUp();

	private:
		static WSAInitializer* m_singleton;

		WSAInitializer();
		~WSAInitializer();

		WSADATA m_wsaData;
		bool m_isInitialized;
	};
}

#endif

#endif
