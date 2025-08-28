#include "MessageIdGenerator.hpp"

namespace tme
{
    uint32_t MessageIdGenerator::GetUniqueId()
    {
        return m_counter.fetch_add(1);
    }
}