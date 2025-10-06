#include "TME/message.hpp"

#include "core/messageFactory.hpp"

namespace tme
{
    std::map<std::string, std::vector<std::pair<std::string, std::pair<size_t, SerializerFunc>>>>& Message::getSerializers()
    {
        static std::map<std::string, std::vector<std::pair<std::string, std::pair<size_t, SerializerFunc>>>> serializers;
        return serializers;
    }

    namespace internal
    {
        void registerMessageType(const std::string& type, std::unique_ptr<Message>(*creator)(const std::vector<uint8_t>&))
        {
            tme::core::MessageFactory::registerMessage(type, creator);
        }

        void serializeField(std::vector<uint8_t>& data, int value)
        {
            data.insert(data.end(), reinterpret_cast<const uint8_t*>(&value),
                reinterpret_cast<const uint8_t*>(&value) + sizeof(value));
        }

        void serializeField(std::vector<uint8_t>& data, float value)
        {
            data.insert(data.end(), reinterpret_cast<const uint8_t*>(&value),
                reinterpret_cast<const uint8_t*>(&value) + sizeof(value));
        }

        void serializeField(std::vector<uint8_t>& data, const std::string& value)
        {
            uint32_t size = static_cast<uint32_t>(value.size());
            data.insert(data.end(), reinterpret_cast<const uint8_t*>(&size),
                reinterpret_cast<const uint8_t*>(&size) + sizeof(size));
            data.insert(data.end(), value.begin(), value.end());
        }
    }
}
