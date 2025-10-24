#include "messageFactory.hpp"

#include <stdexcept>
#include <cstring>

namespace tme::Engine
{
    std::unordered_map<uint32_t, MessageFactory::Creator> MessageFactory::m_registry;

    void MessageFactory::registerMessage(const uint32_t _id, Creator _creator)
    {
        m_registry[_id] = std::move(_creator);
    }

    std::vector<uint8_t> MessageFactory::serialize(const Message& _message)
    {
        TME_ASSERT_REF_PTR_OR_COPIABLE(_message);

        return _message.serialize();
    }

    std::unique_ptr<Message> MessageFactory::deserialize(const std::vector<uint8_t>& _payload)
    {
        TME_ASSERT_REF_PTR_OR_COPIABLE(_payload);

        if (_payload.size() < sizeof(uint32_t))
        {
            throw std::runtime_error("Payload too small");
        }

        uint32_t typeId;
        std::memcpy(&typeId, _payload.data(), sizeof(uint32_t));

        std::unordered_map<uint32_t, Creator>::iterator it = m_registry.find(typeId);
        if (it == m_registry.end())
        {
            throw std::runtime_error("Unknown message type: " + std::to_string(typeId));
        }

        std::vector<uint8_t> data(_payload.begin() + sizeof(uint32_t), _payload.end());
        return it->second(data);
    }
}