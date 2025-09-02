#ifndef TME_MESSAGE_ID_GENERATOR_HPP
#define TME_MESSAGE_ID_GENERATOR_HPP

#include <atomic>
#include <cstdint>

namespace tme
{
    class MessageIdGenerator
    {
    private:
        std::atomic<uint32_t> m_counter;

    public:
        MessageIdGenerator() = default;
        ~MessageIdGenerator() = default;

        uint32_t GetUniqueId();
    };
}

#endif