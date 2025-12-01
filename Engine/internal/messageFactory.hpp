#ifndef TME_ENGINE_MESSAGE_FACTORY_HPP
#define TME_ENGINE_MESSAGE_FACTORY_HPP

#include <functional>
#include <memory>
#include <cstdint>

#include "TME/engine/message.hpp"
#include "messageHeader.hpp"

namespace tme::engine
{
    class MessageFactory
    {
    public:
        using Creator = std::function<std::unique_ptr<Message>(const std::vector<uint8_t>&)>;

        static void registerMessage(const uint32_t _id, Creator _creator);
        static std::vector<uint8_t> serialize(const Message& _message);
        static std::unique_ptr<Message> deserialize(const std::vector<uint8_t>& _payload);

    private:
        static std::unordered_map<uint32_t, Creator> m_registry;
    };
}

#endif