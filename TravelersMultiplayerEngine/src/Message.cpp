#include "Message.hpp"

namespace tme
{
    Message::Message(MessageType type, uint32_t id)
    {
        m_type = type;
        m_id = id;
    }

    MessageType Message::GetType() const 
    { 
        return m_type;
    }

    uint32_t Message::GetId() const
    {
        return m_id;
    }

    const std::vector<uint8_t>& Message::GetPayload() const
    {
        return m_payload;
    }

    void Message::SetPayload(const std::vector<uint8_t>& payload)
    {
        m_payload = payload;
    }

    void Message::SetPayload(const std::string& text)
    {
        m_payload.assign(text.begin(), text.end());
    }
}