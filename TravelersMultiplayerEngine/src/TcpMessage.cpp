#include "TcpMessage.hpp"

#include <cstring>

namespace tme
{
    TcpMessage::TcpMessage(MessageType type, uint32_t id) : Message(type, id)
    {

    }

    std::vector<uint8_t> TcpMessage::Serialize() const
    {
        TcpHeader header;
        header.size = static_cast<uint32_t>(m_payload.size());
        header.type = static_cast<uint16_t>(m_type);
        header.id = m_id;
        header.checksum = CalculateChecksum(m_payload);

        std::vector<uint8_t> result(HEADER_SIZE + m_payload.size());

        std::memcpy(result.data(), &header, HEADER_SIZE);

        if (!m_payload.empty())
        {
            std::memcpy(result.data() + HEADER_SIZE, m_payload.data(), m_payload.size());
        }

        return result;
    }

    bool TcpMessage::Deserialize(const uint8_t* data, size_t size)
    {
        if (size < HEADER_SIZE)
        {
            return false;
        }

        TcpHeader header;
        std::memcpy(&header, data, HEADER_SIZE);

        if (size < HEADER_SIZE + header.size)
        {
            return false;
        }

        m_type = static_cast<MessageType>(header.type);
        m_id = header.id;

        if (header.size > 0)
        {
            m_payload.resize(header.size);
            std::memcpy(m_payload.data(), data + HEADER_SIZE, header.size);
        }
        else
        {
            m_payload.clear();
        }

        uint16_t calculateChecksum = CalculateChecksum(m_payload);
        if (calculateChecksum != header.checksum)
        {
            return false;
        }

        return true;
    }

    size_t TcpMessage::GetHeaderSize() const
    {
        return HEADER_SIZE;
    }

    bool TcpMessage::HasCompleteHeader(const std::vector<uint8_t>& buffer)
    {
        return buffer.size() >= HEADER_SIZE;
    }

    uint32_t TcpMessage::GetPayloadSizeFromHeader(const std::vector<uint8_t>& buffer)
    {
        if (buffer.size() < HEADER_SIZE)
        {
            return 0;
        }

        TcpHeader header;
        std::memcpy(&header, buffer.data(), HEADER_SIZE);
        return header.size;
    }

    bool TcpMessage::HasCompleteMessage(const std::vector<uint8_t>& buffer)
    {
        if (!HasCompleteHeader(buffer))
        {
            return false;
        }

        uint32_t payloadSize = GetPayloadSizeFromHeader(buffer);
        return buffer.size() >= HEADER_SIZE + payloadSize;
    }

    uint16_t TcpMessage::CalculateChecksum(const std::vector<uint8_t>& data) const
    {
        uint16_t checksum = 0;
        for (uint8_t byte : data)
        {
            checksum ^= byte;
        }
        
        return checksum;
    }
}