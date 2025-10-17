#include "TME/message.hpp"

#include "core/messageFactory.hpp"

namespace tme
{
    std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, SerializerFunc>>>>& Message::getSerializers()
    {
        static std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, SerializerFunc>>>> serializers;
        return serializers;
    }

    std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, DeserializerFunc>>>>& Message::getDeserializers()
    {
        static std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, DeserializerFunc>>>> deserializers;
        return deserializers;
    }

    namespace internal
    {
        uint32_t hashTypeName(const char* str)
        {
            uint32_t hash = 2166136261u;
            while (* str)
            {
                hash ^= static_cast<uint32_t>(*str++);
                hash *= 16777619u;
            }
            
            return hash;
        }

        void registerMessageType(const uint32_t id, std::unique_ptr<Message>(*creator)(const std::vector<uint8_t>&))
        {
            tme::core::MessageFactory::registerMessage(id, creator);
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

        void deserializeField(const std::vector<uint8_t>& data, size_t& offset, int& value)
        {
            value = *reinterpret_cast<const int*>(&data[offset]);
            offset += sizeof(int);
        }

        void deserializeField(const std::vector<uint8_t>& data, size_t& offset, float& value)
        {
            value = *reinterpret_cast<const float*>(&data[offset]);
            offset += sizeof(float);
        }

        void deserializeField(const std::vector<uint8_t>& data, size_t& offset, std::string& value)
        {
            uint32_t size = *reinterpret_cast<const uint32_t*>(&data[offset]);
            offset += sizeof(uint32_t);
            value.assign(reinterpret_cast<const char*>(&data[offset]), size);
            offset += size;
        }
    }
}
