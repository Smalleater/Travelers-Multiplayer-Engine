#ifndef TME_TCP_MESSAGE_HPP
#define TME_TCP_MESSAGE_HPP

#include "Message.hpp"

namespace tme
{
    struct TcpHeader
    {
        uint32_t size;
        uint16_t type;
        uint32_t id;
        uint16_t checksum;
    };

    class TcpMessage : public Message
    {
    private:
        uint16_t CalculateChecksum(const std::vector<uint8_t>& data) const;

    public:
        static constexpr size_t HEADER_SIZE = sizeof(TcpHeader);

        TcpMessage(MessageType type, uint32_t id = 0);
        ~TcpMessage() = default;

        std::vector<uint8_t> Serialize() const override;
        bool Deserialize(const uint8_t* data, size_t size) override;

        size_t GetHeaderSize() const override;

        static bool HasCompleteHeader(const std::vector<uint8_t>& buffer);
        static uint32_t GetPayloadSizeFromHeader(const std::vector<uint8_t>& buffer);
        static bool HasCompleteMessage(const std::vector<uint8_t>& buffer);
    };
}

#endif