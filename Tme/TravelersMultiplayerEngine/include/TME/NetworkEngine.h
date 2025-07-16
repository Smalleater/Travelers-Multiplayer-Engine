#pragma once

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

	private:
		static bool m_initialized;
	};
}
