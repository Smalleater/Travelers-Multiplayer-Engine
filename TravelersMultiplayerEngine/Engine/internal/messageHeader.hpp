#ifndef TME_ENGINE_MESSAGE_HEADER_HPP
#define TME_ENGINE_MESSAGE_HEADER_HPP

#include <variant>
#include <string>
#include <unordered_map>

namespace tme::engine
{
    struct MessageHeader
    {
        uint32_t size;
    };
}

#endif