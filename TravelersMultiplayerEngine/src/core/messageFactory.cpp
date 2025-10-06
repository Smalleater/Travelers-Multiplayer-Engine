#include "core/messageFactory.hpp"

#include <stdexcept>

namespace tme::core
{
    std::unordered_map<std::string, MessageFactory::Creator> MessageFactory::m_registry;

    void MessageFactory::registerMessage(const std::string& type, Creator creator)
    {
        m_registry[type] = std::move(creator);
    }

    std::vector<uint8_t> MessageFactory::serialize(const Message& message)
    {
        return message.serialize();
    }

    std::unique_ptr<Message> MessageFactory::deserialize(const std::vector<uint8_t>& payload)
    {
        if (payload.empty())
        {
            throw std::runtime_error("Empty payload");
        }

        size_t offset = 0;
        uint8_t typeLen = payload[offset++];
        std::string typeName(reinterpret_cast<const char*>(&payload[offset]), typeLen);
        offset += typeLen;

        std::unordered_map<std::string, Creator>::iterator it = m_registry.find(typeName);
        if (it == m_registry.end())
        {
            throw std::runtime_error("Unknown message type: " + typeName);
        }

        std::vector<uint8_t> data(payload.begin() + offset, payload.end());
        return it->second(data);
    }
}