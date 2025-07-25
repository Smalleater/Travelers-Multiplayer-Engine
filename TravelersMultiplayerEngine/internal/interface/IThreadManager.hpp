#ifndef TME_I_THREAD_MANAGER_HPP
#define TME_I_THREAD_MANAGER_HPP

#include <functional>

namespace tme
{
    /// @brief Interface for a thread manager that supports job submission and lifecycle management.
    class IThreadManager
    {
    public:
        /// @brief Destructor: default virtual destructor.
        virtual ~IThreadManager() = default;

        /// @brief Initializes the thread manager and starts worker threads.
        virtual void Init() = 0;

        /// @brief Shuts down all worker threads gracefully.
        virtual void Shutdown() = 0;

        /// @brief Submits a job to be executed by the thread pool.
        /// @param job The function to be executed by a worker thread.
        virtual void SubmitJob(std::function<void()> job) = 0;
    };
}

#endif