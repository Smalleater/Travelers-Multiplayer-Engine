#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

#include "Interface/IThreadManager.h"

namespace tme
{
	class ThreadManager : public IThreadManager
	{
	public:
		explicit ThreadManager(size_t threadCount = std::thread::hardware_concurrency());
		~ThreadManager();

		void Init() override;
		void Shutdown() override;
		void SubmitJob(std::function<void()> job) override;

	private:
		std::vector<std::thread> m_workers;
		std::queue<std::function<void()>> m_jobQueue;
		std::mutex m_queueMutex;
		std::condition_variable m_condition;
		std::atomic<bool> m_isRunning;
		size_t m_numThread;
	};
}
