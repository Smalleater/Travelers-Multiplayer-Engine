#include "TME/ThreadManager.h"

namespace tme
{
	ThreadManager::ThreadManager(size_t threadCount)
	{
		m_isRunning = false;
		m_numThread = threadCount;
	}

	ThreadManager::~ThreadManager()
	{
		stop();
	}

	void ThreadManager::start()
	{
		m_isRunning = true;

		for (size_t i = 0; i < m_numThread; i++)
		{
			m_workers.emplace_back([this]()
				{
					while (true)
					{
						std::function<void()> job;
						{
							std::unique_lock<std::mutex> lock(m_queueMutex);
							m_condition.wait(lock, [this]()
								{
									return !m_isRunning || !m_jobQueue.empty();
								});

							if (!m_isRunning && m_jobQueue.empty())
							{
								return;
							}

							job = std::move(m_jobQueue.front());
							m_jobQueue.pop();
						}

						if (job)
						{
							job();
						}
					}
				});
		}
	}

	void ThreadManager::stop()
	{
		{
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_isRunning = false;
		}

		m_condition.notify_all();

		for (std::thread& worker : m_workers)
		{
			if (worker.joinable())
			{
				worker.join();
			}
		}

		m_workers.clear();
	}

	void ThreadManager::addJob(std::function<void()> job)
	{
		{
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_jobQueue.push(std::move(job));
		}

		m_condition.notify_one();
	}
}
