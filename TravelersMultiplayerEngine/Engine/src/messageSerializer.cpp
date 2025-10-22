#include "messageSerializer.hpp"

#include <stdexcept>
#include <cstring>

namespace tme::engine
{
    std::vector<uint8_t> MessageSerializer::serializePayload(const Message& _message)
    {
        return MessageFactory::serialize(_message);
    }

    std::unique_ptr<Message> MessageSerializer::deserializePayload(const std::vector<uint8_t>& _payload)
    {
        return MessageFactory::deserialize(_payload);
    }

    std::vector<uint8_t> MessageSerializer::serializeForNetwork(const std::vector<uint8_t>& _payload, bool _internal)
    {
        MessageHeader header;
        header.size = static_cast<uint32_t>(_payload.size());
        header.typeFlag = _internal ? INTERNAL_MESSAGE : USER_MESSAGE;

        std::vector<uint8_t> data(sizeof(MessageHeader));
        std::memcpy(data.data(), &header, sizeof(header));
        data.insert(data.end(), _payload.begin(), _payload.end());

        return data;
    }

    std::pair<MessageHeader, std::vector<uint8_t>> MessageSerializer::deserializeFromNetwork(const std::vector<uint8_t>& _data)
    {
        if(_data.size() < sizeof(MessageHeader))
        {
            throw std::runtime_error("Data too small");
        }

        MessageHeader header;
        std::memcpy(&header, _data.data(), sizeof(header));
        std::vector<uint8_t> payload(_data.begin() + sizeof(MessageHeader), _data.end());

        return {header, payload};
    }
}