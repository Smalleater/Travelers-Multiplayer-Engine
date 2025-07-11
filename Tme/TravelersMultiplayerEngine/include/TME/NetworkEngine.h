#pragma once

#include "TME/TMEExport.h"
#include "TME/ErrorCodes.h"

namespace tme
{
	class TME_API NetworkEngine
	{
	public:
		static NetworkError Init();
		static NetworkError Shutdown();
		static NetworkError EnsureInitialized();

		static bool IsInitialized();

	private:
		static bool m_initialized;

		static bool HasAnyConsoleOutput();
	};
}
