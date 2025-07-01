#pragma once

namespace tme
{
	class Engine
	{
	public:
		static bool start();
		static bool stop();

	private:
		static bool m_isRunning;
	};
}
