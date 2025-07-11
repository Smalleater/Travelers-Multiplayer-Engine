#pragma once

#include <functional>

namespace tme
{
	class IThreadManager
	{
	public:
		virtual ~IThreadManager() = default;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void SubmitJob(std::function<void()> job) = 0;
	};
}
