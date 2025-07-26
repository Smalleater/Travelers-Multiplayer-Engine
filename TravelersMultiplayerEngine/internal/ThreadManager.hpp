#ifndef TME_THREAD_MANAGER_HPP
#define TME_THREAD_MANAGER_HPP

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "interfaces/IThreadManager.hpp"
 
namespace tme
{
    /// @brief Concrete implementation of IThreadManager that manages a pool of worker threads.
    ///
    /// Jobs submitted are executed asynchronously using a thread-safe queue.
    class ThreadManager : public IThreadManager
    {
    private:
    /// @brief Vector containing all worker threads.
    std::vector<std::thread> m_workers;

    /// @brief Queue of pending jobs to be executed by workers.
    std::queue<std::function<void()>> m_jobQueue;

    /// @brief Mutex to protect access to the job queue.
    std::mutex m_queueMutex;

    /// @brief Condition variable to notify workers of new jobs.
    std::condition_variable m_condition;

    /// @brief Flag indicating whether the thread pool is currently running.
    std::atomic<bool> m_isRunning;

    /// @brief The number of worker threads in the pool.
    size_t m_numThread;

    public:
    /// @brief Constructor: Initializes the ThreadManager with a specified number of threads.
    /// @param threadCount The number of worker threads to create.
    /// Defaults to the number of hardware threads available.
    explicit ThreadManager(size_t threadCount = std::thread::hardware_concurrency());

    /// @brief Destructor: Shuts down all worker threads.
    ~ThreadManager();

    /// @brief Initializes the thread pool and starts all worker threads.
    void Init() override;

    /// @brief Signals all threads to terminate and waits for them to finish.
    void Shutdown() override;

    /// @brief Submits a job (function) to be executed asynchronously by the thread pool.
    /// @param job The function to be executed.
    void SubmitJob(std::function<void()> job) override;
    };
}

#endif