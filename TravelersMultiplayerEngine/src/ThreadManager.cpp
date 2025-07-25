#include "ThreadManager.hpp"

namespace tme
{
    // Constructor: Initializes the thread manager with a specified number of threads
    ThreadManager::ThreadManager(size_t threadCount)
    {
        m_isRunning = false;

        // If no thread count is specified, default to 1
        m_numThread = (threadCount == 0) ? 1 : threadCount;
    }

    // Destructor: Ensures Threads are properly shut down
    ThreadManager::~ThreadManager()
    {
        Shutdown();
    }

    // Initializes and starts the worker threads
    void ThreadManager::Init()
    {
        m_isRunning = true;

        for (size_t i = 0; i < m_numThread; i++)
        {
            // Launch each worker thread
            m_workers.emplace_back([this]()
            {
                while (true)
                {
                    std::function<void()> job;
                    {
                        // Acquire lock to access the job queue
                        std::unique_lock<std::mutex> lock(m_queueMutex);

                        // Wait until there's a job or a shutdown is triggered
                        m_condition.wait(lock, [this]()
                        {
                            return !m_isRunning || !m_jobQueue.empty();
                        });

                        // If we're shutting down and there are no more jobs, exit the loop
                        if (!m_isRunning && m_jobQueue.empty())
                        {
                            return;
                        }

                        // Retrieve the next job from the queue
                        job = std::move(m_jobQueue.front());
                        m_jobQueue.pop();
                    }

                    // Execute the job if valid
                    if (job)
                    {
                        job();
                    }
                }
            });
        }
    }

    // Shuts down all worker threads gracefully
    void ThreadManager::Shutdown()
    {
        {
            // Set running flag to false to signal shutdown
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_isRunning = false;
        }

        // Notify all threads to wake up
        m_condition.notify_all();

        // Join all threads
        for (std::thread& worker : m_workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
        
        // Clear the list of workers
        m_workers.clear();
    }

    // Submits a job to the thread pool for execution
    void ThreadManager::SubmitJob(std::function<void()> job)
    {
        {
            // Add the job to the queue
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_jobQueue.push(std::move(job));
        }

        // Notify one worker thread to process the job
        m_condition.notify_one();
    }
}