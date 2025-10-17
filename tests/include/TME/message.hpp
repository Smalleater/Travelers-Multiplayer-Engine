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

        static std::map<std::string, std::vector<std::pair<std::string, std::pair<size_t, SerializerFunc>>>>& getSerializers();
        static std::map<std::string, std::vector<std::pair<std::string, std::pair<size_t, DeserializerFunc>>>>& getDeserializers();
    };

    namespace internal 
    {
        TME_API void registerMessageType(const std::string& type,
            std::unique_ptr<Message>(*creator)(const std::vector<uint8_t>&));
        
        TME_API void serializeField(std::vector<uint8_t>& data, int value);
        TME_API void serializeField(std::vector<uint8_t>& data, float value);
        TME_API void serializeField(std::vector<uint8_t>& data, const std::string& value);

        template<typename T>
        void registerSerializer(const std::string& messageType, const std::string& fieldName, size_t fieldOffset)
        {
            auto& serializers = Message::getSerializers();
            serializers[messageType].emplace_back(fieldName, std::make_pair(fieldOffset, [fieldOffset](const void* base, std::vector<uint8_t>& data) {
                const T* field = reinterpret_cast<const T*>(static_cast<const char*>(base) + fieldOffset);
                serializeField(data, *field);
            }));
        }

        TME_API void deserializeField(const std::vector<uint8_t>& data, size_t& offset, int& value);
        TME_API void deserializeField(const std::vector<uint8_t>& data, size_t& offset, float& value);
        TME_API void deserializeField(const std::vector<uint8_t>& data, size_t& offset, std::string& value);

        template<typename T>
        void registerDeserializer(const std::string& messageType, const std::string& fieldName, size_t fieldOffset)
        {
            auto& deserializers = Message::getDeserializers();
            deserializers[messageType].emplace_back(fieldName, std::make_pair(fieldOffset, 
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
        using CurrentMessageType = MessageType;

#define FIELD(type, name) \
    type name; \
    private: \
        struct name##_Registrar \
        { \
            name##_Registrar() \
            { \
                const auto offset = reinterpret_cast<size_t>(&(static_cast<CurrentMessageType*>(nullptr)->name)); \
                tme::internal::registerSerializer<type>(MESSAGE_TYPE_NAME, #name, offset); \
                tme::internal::registerDeserializer<type>(MESSAGE_TYPE_NAME, #name, offset); \
            } \
        }; \
        inline static name##_Registrar name##_reg; \
    public: \

#define DECLARE_MESSAGE_END() \
        std::string getType() const override { return MESSAGE_TYPE_NAME; } \
        std::vector<uint8_t> serialize() const override \
        { \
            std::vector<uint8_t> data; \
            std::string type = getType(); \
            uint8_t typeSize = static_cast<uint8_t>(type.size()); \
            data.push_back(typeSize); \
            data.insert(data.end(), type.begin(), type.end()); \
            auto& serializers = getSerializers(); \
            auto it = serializers.find(MESSAGE_TYPE_NAME); \
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
            uint8_t typeSize = payload[0]; \
            size_t offset = 1 + typeSize; \
            auto& deserializers = getDeserializers(); \
            auto it = deserializers.find(MESSAGE_TYPE_NAME); \
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
            Register() { tme::internal::registerMessageType(MESSAGE_TYPE_NAME, CurrentMessageType::createFromBytes); } \
        }; \
        inline static Register _register{}; \
    }; \
}

#endif