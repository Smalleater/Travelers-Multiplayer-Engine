#pragma once

#include "../TMEExport.h"

namespace tme
{
	class TME_API Engine
	{
	public:
		static bool start();
		static bool stop();

	private:
		static bool m_isRunning;
	};
}
