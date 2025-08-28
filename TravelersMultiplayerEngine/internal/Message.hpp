#ifndef TME_MESSAGE_HPP
#define TME_MESSAGE_HPP

#include <vector>
#include <cstdint>
#include <string>

namespace tme
{
    enum class MessageType : uint16_t
    {
        CONNECT = 0,
        DISCONNECT,
        PING,
        PONG,
        HEARTBEAT,
        GAME_DATA,
        UNKNOWN,
    };

    class Message
    {
    protected:
        MessageType m_type;
        uint32_t m_id;
        std::vector<uint8_t> m_payload;

    public:
        Message(MessageType type, uint32_t id = 0);
        virtual ~Message() = default;

        virtual std::vector<uint8_t> Serialize() const = 0;
        virtual bool Deserialize(const uint8_t* data, size_t size) = 0;
        virtual size_t GetHeaderSize() const = 0;

        MessageType GetType() const;
        uint32_t GetId() const;
        const std::vector<uint8_t>& GetPayload() const;

        void SetPayload(const std::vector<uint8_t>& payload);
        void SetPayload(const std::string& text);
    };
}

#endif