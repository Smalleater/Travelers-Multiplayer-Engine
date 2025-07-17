#pragma once

#include <memory>

#include "TME/TMEExport.h"
#include "TME/ErrorCodes.h"

namespace tme
{
	class TME_API NetworkEngine
	{
	public:
		static ErrorCodes Init();
		static ErrorCodes Shutdown();
		static ErrorCodes EnsureInitialized();

		static bool IsInitialized();

		static ErrorCodes StartServer();
		static ErrorCodes StartClient();

	private:
		static bool m_initialized;
		static void* m_networkManager;
	};
}
