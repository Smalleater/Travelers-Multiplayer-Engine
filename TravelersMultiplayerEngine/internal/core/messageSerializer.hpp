#ifndef TME_CORE_MESSAGE_SERIALIZER_HPP
#define TME_CORE_MESSAGE_SERIALIZER_HPP

#include "core/messageFactory.hpp"

namespace tme::core
{
    class MessageSerializer
    {
    public:
        static std::vector<uint8_t> serializePayload(const Message& message);
        static std::unique_ptr<Message> deserializePayload(const std::vector<uint8_t>& payload);
        static std::vector<uint8_t> serializeForNetwork(const std::vector<uint8_t>& payload, bool internal = false);
        static std::pair<MessageHeader, std::vector<uint8_t>> deserializeFromNetwork(const std::vector<uint8_t>& data);
    };    
}

#endif