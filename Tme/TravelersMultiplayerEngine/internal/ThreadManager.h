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
	/// <summary>
	/// Concrete implementation of IThreadManager that manages a pool of worker threads
	/// Jobs submitted are executed asynchronously using a thread-safe queue
	/// </summary>
	class ThreadManager : public IThreadManager
	{
	public:
		/// <summary>
		/// Constructs the ThreadManager with a specified number of threads
		/// Defaults to the number of hardware threads available
		/// </summary>
		/// <param name="threadCount">The number of threads to create in the pool</param>
		explicit ThreadManager(size_t threadCount = std::thread::hardware_concurrency());

		/// <summary>
		/// Destructor that ensures proper shutdown of all worker threads
		/// </summary>
		~ThreadManager();

		/// <summary>
		/// Initializes the thread pool and starts all worker threads
		/// </summary>
		void Init() override;

		/// <summary>
		/// Signals all threads to terminate and waits for them to finish
		/// </summary>
		void Shutdown() override;

		/// <summary>
		/// Submits a job (function) to be executed asynchronously by the thread pool
		/// </summary>
		/// <param name="job">The function to be executed</param>
		void SubmitJob(std::function<void()> job) override;

	private:
		/// <summary>
		/// Vector containing all worker threads
		/// </summary>
		std::vector<std::thread> m_workers;

		/// <summary>
		/// Queue of pending jobs to be executed by workers
		/// </summary>
		std::queue<std::function<void()>> m_jobQueue;

		/// <summary>
		/// Mutex to protect access to the job queue
		/// </summary>
		std::mutex m_queueMutex;

		/// <summary>
		/// Condition variable to notify workers of new jobs
		/// </summary>
		std::condition_variable m_condition;

		/// <summary>
		/// Flag indicating whether the thread pool is currently running
		/// </summary>
		std::atomic<bool> m_isRunning;

		/// <summary>
		/// The number of worker threads in the pool
		/// </summary>
		size_t m_numThread;
	};
}
