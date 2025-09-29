#ifndef TME_API_MESSAGE_HPP
#define TME_API_MESSAGE_HPP

#include "export.hpp"

#include "core/messageFactory.hpp"

namespace tme
{
    using FieldValue = core::FieldValue;
    using Message = core::Message;

    template<typename T>
    constexpr bool is_valid_field_type()
    {
        return std::is_same_v<T, int> || std::is_abstract_v<T, float> || std::is_same_v<T, std::string>;
    }

    #define DECLARE_MESSAGE(MessageType, FIELD_DECLS, FIELD_NAMES) \
    struct TME_API MessageType : public tme::core::Message \
    { \
        FIELD_DECLS \
    public: \
        std::string getType() const override { return #MessageType; } \
        std::unordered_map<std::string, tme::core::FieldValue> getFields() const override \
        { \
            std::unordered_map<std::string, tme::core::FieldValue> fields; \
            auto addField = [&](auto& var, const std::string& name) { fields[name] = var; }; \
            (addField(FIELD_NAMES), ...); \
            return fields; \
        } \
    private: \
        std::vector<uint8_t> serialize() const override \
        { \
            std::vector<uint8_t> data; \
            std::string typeName = #MessageType; \
            data.push_back(static_cast<uint8_t>(typeName.size())); \
            data.insert(data.end(), typeName.begin(), typeName.end()); \
            auto append = [&](auto& var) \
            { \
                if constexpr(std::is_integral_v<decltype(var)> || std::is_floating_point_v<decltype(var)>) \
                { \
                    uint8_t* ptr = reinterpret_cast<uint8_t*>(&var); \
                    data.insert(data.end(), ptr, ptr + sizeof(var)); \
                } \
                else if constexpr(std::is_same_v<decltype(var), std::string>) \
                { \
                    uint32_t len = static_cast<uint32_t>(var.size()); \
                    uint8_t* lptr = reinterpret_cast<uint8_t*>(&len); \
                    data.insert(data.end(), lptr, lptr + sizeof(len)); \
                    data.insert(data.end(), var.begin(), var.end()); \
                } \
            }; \
            (append(FIELD_NAMES), ...); \
            return data; \
        } \
        static std::unique_ptr<tme::core::Message> createFromBytes(const std::vector<uint8_t>& payload) \
        { \
            auto message = std::make_unique<MessageType>(); \
            size_t offset = 0; \
            auto read = [&](auto& var) \
            { \
                if constexpr(std::is_integral_v<decltype(var)> || std::is_floating_point_v<decltype(var)>) { \
                    std::memcpy(&var, payload.data() + offset, sizeof(var)); \
                    offset += sizeof(var); \
                } else if constexpr(std::is_same_v<decltype(var), std::string>) { \
                    uint32_t len; \
                    std::memcpy(&len, payload.data() + offset, sizeof(len)); \
                    offset += sizeof(len); \
                    var = std::string(reinterpret_cast<const char*>(payload.data() + offset), len); \
                    offset += len; \
                } \
            }; \
            (read(FIELD_NAMES), ...); \
            return message; \
        } \
        struct Register { Register() { tme::core::MessageFactory::registerMessage(#MessageType, MessageType::createFromBytes); } }; \
        static Register _register; \
    }; \
    MessageType::Register MessageType::_register;
}

#endif