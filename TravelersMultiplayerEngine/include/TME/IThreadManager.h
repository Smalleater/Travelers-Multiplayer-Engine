#pragma once

#include <functional>

namespace tme
{
	class IThreadManager
	{
	public:
		virtual ~IThreadManager() = default;

		virtual void start() = 0;
		virtual void stop() = 0;
		virtual void addJob(std::function<void()> job) = 0;
	};
}
