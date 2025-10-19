#ifndef TME_API_MESSAGE_HPP
#define TME_API_MESSAGE_HPP

#include "export.hpp"

#include <string>
#include <map>
#include <vector>
#include <variant>
#include <memory>
#include <cstdint>
#include <functional>
#include <algorithm>

namespace tme
{
    using FieldValue = std::variant<int, float, std::string>;
    using SerializerFunc = std::function<void(const void*, std::vector<uint8_t>&)>;
    using DeserializerFunc = std::function<void(const void*, const std::vector<uint8_t>&, size_t&)>;

    class TME_API Message
    {
    public:
        virtual ~Message() = default;
        virtual std::string getType() const = 0;
        virtual std::vector<uint8_t> serialize() const = 0;

        static std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, SerializerFunc>>>>& getSerializers();
        static std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, DeserializerFunc>>>>& getDeserializers();
    };

    namespace internal 
    {
        TME_API uint32_t hashTypeName(const char* str);

        TME_API void registerMessageType(const uint32_t id,
            std::unique_ptr<Message>(*creator)(const std::vector<uint8_t>&));
        
        TME_API void serializeField(std::vector<uint8_t>& data, int value);
        TME_API void serializeField(std::vector<uint8_t>& data, float value);
        TME_API void serializeField(std::vector<uint8_t>& data, const std::string& value);

        template<typename T>
        void registerSerializer(const uint32_t messageId, const std::string& fieldName, size_t fieldOffset)
        {
            auto& serializers = Message::getSerializers();
            serializers[messageId].emplace_back(fieldName, std::make_pair(fieldOffset, [fieldOffset](const void* base, std::vector<uint8_t>& data) {
                const T* field = reinterpret_cast<const T*>(static_cast<const char*>(base) + fieldOffset);
                serializeField(data, *field);
            }));
        }

        TME_API void deserializeField(const std::vector<uint8_t>& data, size_t& offset, int& value);
        TME_API void deserializeField(const std::vector<uint8_t>& data, size_t& offset, float& value);
        TME_API void deserializeField(const std::vector<uint8_t>& data, size_t& offset, std::string& value);

        template<typename T>
        void registerDeserializer(const uint32_t messageId, const std::string& fieldName, size_t fieldOffset)
        {
            auto& deserializers = Message::getDeserializers();
            deserializers[messageId].emplace_back(fieldName, std::make_pair(fieldOffset, 
                [fieldOffset](const void* base, const std::vector<uint8_t>& data, size_t& offset) {
                    T* field = reinterpret_cast<T*>(static_cast<char*>(const_cast<void*>(base)) + fieldOffset);
                    deserializeField(data, offset, *field);
                }));
        }
    }
}

#define DECLARE_MESSAGE_BEGIN(MessageType) \
namespace tme { \
    struct MessageType : public Message \
    { \
    public: \
        static constexpr const char* MESSAGE_TYPE_NAME = #MessageType; \
        inline static uint32_t MESSAGE_TYPE_ID = tme::internal::hashTypeName(MESSAGE_TYPE_NAME); \
        using CurrentMessageType = MessageType;

#define FIELD(type, name) \
    type name; \
    private: \
        struct name##_Registrar \
        { \
            name##_Registrar() \
            { \
                const auto offset = reinterpret_cast<size_t>(&(static_cast<CurrentMessageType*>(nullptr)->name)); \
                tme::internal::registerSerializer<type>(MESSAGE_TYPE_ID, #name, offset); \
                tme::internal::registerDeserializer<type>(MESSAGE_TYPE_ID, #name, offset); \
            } \
        }; \
        inline static name##_Registrar name##_reg; \
    public: \

#define DECLARE_MESSAGE_END() \
        std::string getType() const override { return MESSAGE_TYPE_NAME; } \
        std::vector<uint8_t> serialize() const override \
        { \
            std::vector<uint8_t> data; \
            uint32_t typeId = MESSAGE_TYPE_ID; \
            data.insert(data.end(), \
                reinterpret_cast<const uint8_t*>(&typeId), \
                reinterpret_cast<const uint8_t*>(&typeId) + sizeof(typeId)); \
            auto& serializers = getSerializers(); \
            auto it = serializers.find(MESSAGE_TYPE_ID); \
            if (it != serializers.end()) \
            { \
                for (const auto& [fieldName, fieldData] : it->second) \
                { \
                    fieldData.second(this, data); \
                } \
            } \
            return data; \
        } \
        static std::unique_ptr<Message> createFromBytes(const std::vector<uint8_t>& payload) \
        { \
            std::unique_ptr<CurrentMessageType> message = std::make_unique<CurrentMessageType>(); \
            size_t offset = sizeof(uint32_t); \
            auto& deserializers = getDeserializers(); \
            auto it = deserializers.find(MESSAGE_TYPE_ID); \
            if (it != deserializers.end()) \
            { \
                for (const auto& [fieldName, fieldData] : it->second) \
                { \
                    fieldData.second(message.get(), payload, offset); \
                } \
            } \
            return message; \
        } \
    private: \
        struct Register \
        { \
            Register() { tme::internal::registerMessageType(MESSAGE_TYPE_ID, CurrentMessageType::createFromBytes); } \
        }; \
        inline static Register _register{}; \
    }; \
}

#endif