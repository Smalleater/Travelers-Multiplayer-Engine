#include "core/messageFactory.hpp"

#include <stdexcept>
#include <cstring>

namespace tme::core
{
    std::unordered_map<uint32_t, MessageFactory::Creator> MessageFactory::m_registry;

    void MessageFactory::registerMessage(const uint32_t id, Creator creator)
    {
        m_registry[id] = std::move(creator);
    }

    std::vector<uint8_t> MessageFactory::serialize(const Message& message)
    {
        return message.serialize();
    }

    std::unique_ptr<Message> MessageFactory::deserialize(const std::vector<uint8_t>& payload)
    {
        if (payload.size() < sizeof(uint32_t))
        {
            throw std::runtime_error("Payload too small");
        }

        uint32_t typeId;
        std::memcpy(&typeId, payload.data(), sizeof(uint32_t));

        std::unordered_map<uint32_t, Creator>::iterator it = m_registry.find(typeId);
        if (it == m_registry.end())
        {
            throw std::runtime_error("Unknown message type: " + std::to_string(typeId));
        }

        std::vector<uint8_t> data(payload.begin() + sizeof(uint32_t), payload.end());
        return it->second(data);
    }
}