#include "core/messageSerializer.hpp"

#include <stdexcept>
#include <cstring>

namespace tme::core
{
    std::vector<uint8_t> MessageSerializer::serializePayload(const Message& message)
    {
        return MessageFactory::serialize(message);
    }

    std::unique_ptr<Message> MessageSerializer::deserializePayload(const std::vector<uint8_t>& payload)
    {
        return MessageFactory::deserialize(payload);
    }

    std::vector<uint8_t> MessageSerializer::serializeForNetwork(const std::vector<uint8_t>& payload, bool internal)
    {
        MessageHeader header;
        header.size = static_cast<uint32_t>(payload.size());
        header.typeFlag = internal ? INTERNAL_MESSAGE : USER_MESSAGE;

        std::vector<uint8_t> data(sizeof(MessageHeader));
        std::memcpy(data.data(), &header, sizeof(header));
        data.insert(data.end(), payload.begin(), payload.end());

        return data;
    }

    std::pair<MessageHeader, std::vector<uint8_t>> MessageSerializer::deserializeFromNetwork(const std::vector<uint8_t>& data)
    {
        if(data.size() < sizeof(MessageHeader))
        {
            throw std::runtime_error("Data too small");
        }

        MessageHeader header;
        std::memcpy(&header, data.data(), sizeof(header));
        std::vector<uint8_t> payload(data.begin() + sizeof(MessageHeader), data.end());

        return {header, payload};
    }
}