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

    class TME_API Message
    {
    protected:
        static std::map<std::string, std::vector<std::pair<std::string, std::pair<size_t, SerializerFunc>>>>& getSerializers();
    public:
        virtual ~Message() = default;
        virtual std::string getType() const = 0;
        virtual std::vector<uint8_t> serialize() const = 0;
    };

    template<typename T>
    constexpr bool is_valid_field_type() {
        return std::is_same_v<T, int> ||
               std::is_same_v<T, float> ||
               std::is_same_v<T, std::string>;
    }

    namespace internal 
    {
        void registerMessageType(const std::string& type,
            std::unique_ptr<Message>(*creator)(const std::vector<uint8_t>&));
        
        void serializeField(std::vector<uint8_t>& data, int value);
        void serializeField(std::vector<uint8_t>& data, float value);
        void serializeField(std::vector<uint8_t>& data, const std::string& value);

        template<typename T>
        void registerField(const std::string& messageType, const std::string& fieldName, size_t offset)
        {
            auto& serializers = Message::getSerializers();
            serializers[messageType].emplace_back(fieldName, std::make_pair(offset, [offset](const void* base, std::vector<uint8_t>& data) {
                const T* field = reinterpret_cast<const T*>(static_cast<const char*>(base) + offset);
                serializeField(data, *field);
            }));
        }
    }
}

#define DECLARE_MESSAGE_BEGIN(MessageType) \
namespace tme { \
    struct TME_API MessageType : public Message \
    { \
    public: \
        static constexpr const char* MESSAGE_TYPE_NAME = #MessageType; \
        using CurrentMessageType = MessageType;

#define FIELD(type, name) \
    type name; \
    struct name##_Registrar \
    { \
        name##_Registrar() \
        { \
            tme::internal::registerField<type>(MESSAGE_TYPE_NAME, #name, offsetof(CurrentMessageType, name)); \
        } \
    }; \
    static name##_Registrar name##_reg;

#define DECLARE_MESSAGE_END() \
        std::string getType() const override { return MESSAGE_TYPE_NAME; } \
        std::vector<uint8_t> serialize() const override \
        { \
            std::vector<uint8_t> data; \
            std::string type = getType(); \
            uint32_t typeSize = static_cast<uint32_t>(type.size()); \
            data.insert(data.end(), reinterpret_cast<const uint8_t*>(&typeSize), \
                reinterpret_cast<const uint8_t*>(&typeSize) + sizeof(typeSize)); \
            data.insert(data.end(), type.begin(), type.end()); \
            auto& serializers = getSerializers(); \
            auto it = serializers.find(MESSAGE_TYPE_NAME); \
            if (it != serializers.end()) \
            { \
                auto fields = it->second; \
                std::sort(fields.begin(), fields.end(), [](const auto& a, const auto& b) { \
                    return a.first < b.first; \
                }); \
                for (const auto& [fieldName, fieldData] : fields) \
                { \
                    fieldData.second(this, data); \
                } \
            } \
            return data; \
        } \
        static std::unique_ptr<Message> createFromBytes(const std::vector<uint8_t>& payload); \
    private: \
        struct Register \
        { \
            Register() { tme::internal::registerMessageType(MESSAGE_TYPE_NAME, CurrentMessageType::createFromBytes); } \
        }; \
        static Register _register; \
    }; \
}

#endif