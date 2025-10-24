#ifndef TME_ENGINE_MESSAGE_HEADER_HPP
#define TME_ENGINE_MESSAGE_HEADER_HPP

#include <variant>
#include <string>
#include <unordered_map>

namespace tme::Engine
{
    constexpr uint8_t USER_MESSAGE = 0;
    constexpr uint8_t INTERNAL_MESSAGE = 1;

    struct MessageHeader
    {
        uint32_t size;
        uint8_t typeFlag;
    };
}

#endif