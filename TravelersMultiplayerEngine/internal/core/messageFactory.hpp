#ifndef TME_CORE_MESSAGE_FACTORY_HPP
#define TME_CORE_MESSAGE_FACTORY_HPP

#include <functional>
#include <memory>
#include <cstdint>

#include "TME/message.hpp"
#include "core/messageHeader.hpp"

namespace tme::core
{
    class MessageFactory
    {
    public:
        using Creator = std::function<std::unique_ptr<Message>(const std::vector<uint8_t>&)>;

    private:
        static std::unordered_map<std::string, Creator> m_registry;

    public:
        static void registerMessage(const std::string& type, Creator creator);
        static std::unique_ptr<Message> deserialize(const std::vector<uint8_t>& payload);
        static std::vector<uint8_t> serialize(const Message& message);
    };
}

#endif