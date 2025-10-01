#ifndef TME_API_MESSAGE_HPP
#define TME_API_MESSAGE_HPP

#include "export.hpp"

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <memory>
#include <cstdint>

namespace tme
{
    using FieldValue = std::variant<int, float, std::string>;

    class TME_API Message
    {
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
        TME_API void registerMessageType(
            const std::string& type,
            std::unique_ptr<Message>(*)(const std::vector<uint8_t>&));
    }
}

#ifdef TME_EXPORTS
    #define DECLARE_MESSAGE_API TME_API
#else
    #define DECLARE_MESSAGE_API
#endif

#define DECLARE_MESSAGE(MessageType, ...) \
namespace tme { \
    struct DECLARE_MESSAGE_API MessageType : public Message \
    { \
        __VA_ARGS__ \
    public: \
        std::string getType() const override { return #MessageType; } \
        std::vector<uint8_t> serialize() const override; \
        static std::unique_ptr<Message> createFromBytes(const std::vector<uint8_t>& payload); \
    private: \
        struct Register \
        { \
            Register() { tme::internal::registerMessageType(#MessageType, MessageType::createFromBytes); } \
        }; \
        static Register _register; \
    }; \
    MessageType::Register MessageType::_register; \
}

#endif