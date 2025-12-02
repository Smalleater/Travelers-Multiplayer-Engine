#include "messageSerializer.hpp"

#include <stdexcept>
#include <cstring>

#include "TRA/debugUtils.hpp"

namespace tra::engine
{
	std::vector<uint8_t> MessageSerializer::serializePayload(const Message& _message)
	{
		TRA_ASSERT_REF_PTR_OR_COPIABLE(_message);

		return MessageFactory::serialize(_message);
	}

	std::unique_ptr<Message> MessageSerializer::deserializePayload(const std::vector<uint8_t>& _payload)
	{
		TRA_ASSERT_REF_PTR_OR_COPIABLE(_payload);

		return MessageFactory::deserialize(_payload);
	}

	std::vector<uint8_t> MessageSerializer::serializeForNetwork(const std::vector<uint8_t>& _payload, bool _internal)
	{
		TRA_ASSERT_REF_PTR_OR_COPIABLE(_payload);

		MessageHeader header;
		header.size = static_cast<uint32_t>(_payload.size());

		std::vector<uint8_t> data(sizeof(MessageHeader));
		std::memcpy(data.data(), &header, sizeof(header));
		data.insert(data.end(), _payload.begin(), _payload.end());

		return data;
	}

	bool MessageSerializer::getPayloadFromNetworkBuffer(const std::vector<uint8_t>& _buffer, 
		std::vector<uint8_t>& _outPayload, size_t& _outConsumedBytes)
	{
		TRA_ASSERT_REF_PTR_OR_COPIABLE(_buffer);
		TRA_ASSERT_REF_PTR_OR_COPIABLE(_outPayload);

		_outConsumedBytes = 0;
		_outPayload.clear();

		if (_buffer.size() < sizeof(MessageHeader))
		{
			return false;
		}

		MessageHeader header;
		std::memcpy(&header, _buffer.data(), sizeof(MessageHeader));
		if (_buffer.size() < sizeof(MessageHeader) + header.size)
		{
			return false;
		}

		_outPayload.resize(header.size);
		std::memcpy(_outPayload.data(), _buffer.data() + sizeof(MessageHeader), header.size);

		_outConsumedBytes = sizeof(MessageHeader) + header.size;

		return true;
	}
}