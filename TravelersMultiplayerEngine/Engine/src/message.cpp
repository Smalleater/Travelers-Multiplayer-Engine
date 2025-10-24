#include "TME/engine/message.hpp"

#include "messageFactory.hpp"

namespace tme::engine
{
    std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, SerializerFunc>>>>& Message::getSerializers()
    {
        static std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, SerializerFunc>>>> serializers;
        return serializers;
    }

    std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, DeserializerFunc>>>>& Message::getDeserializers()
    {
        static std::map<uint32_t, std::vector<std::pair<std::string, std::pair<size_t, DeserializerFunc>>>> deserializers;
        return deserializers;
    }

    namespace internal
    {
        uint32_t hashTypeName(const char* _str)
        {
            uint32_t hash = 2166136261u;
            while (* _str)
            {
                hash ^= static_cast<uint32_t>(*_str++);
                hash *= 16777619u;
            }
            
            return hash;
        }

        void registerMessageType(const uint32_t _id, std::unique_ptr<Message>(*_creator)(const std::vector<uint8_t>&))
        {
            TME_ASSERT_REF_PTR_OR_COPIABLE(_creator);

            MessageFactory::registerMessage(_id, _creator);
        }

        void serializeField(std::vector<uint8_t>& _data, int _value)
        {
            TME_ASSERT_REF_PTR_OR_COPIABLE(_data);

            _data.insert(_data.end(), reinterpret_cast<const uint8_t*>(&_value),
                reinterpret_cast<const uint8_t*>(&_value) + sizeof(_value));
        }

        void serializeField(std::vector<uint8_t>& _data, float _value)
        {
            TME_ASSERT_REF_PTR_OR_COPIABLE(_data);

            _data.insert(_data.end(), reinterpret_cast<const uint8_t*>(&_value),
                reinterpret_cast<const uint8_t*>(&_value) + sizeof(_value));
        }

        void serializeField(std::vector<uint8_t>& _data, const std::string& _value)
        {
            TME_ASSERT_REF_PTR_OR_COPIABLE(_data);
            TME_ASSERT_REF_PTR_OR_COPIABLE(_value);

            uint32_t size = static_cast<uint32_t>(_value.size());
            _data.insert(_data.end(), reinterpret_cast<const uint8_t*>(&size),
                reinterpret_cast<const uint8_t*>(&size) + sizeof(size));
            _data.insert(_data.end(), _value.begin(), _value.end());
        }

        void deserializeField(const std::vector<uint8_t>& _data, size_t& _offset, int& _value)
        {
            TME_ASSERT_REF_PTR_OR_COPIABLE(_data);

            _value = *reinterpret_cast<const int*>(&_data[_offset]);
            _offset += sizeof(int);
        }

        void deserializeField(const std::vector<uint8_t>& _data, size_t& _offset, float& _value)
        {
            TME_ASSERT_REF_PTR_OR_COPIABLE(_data);

            _value = *reinterpret_cast<const float*>(&_data[_offset]);
            _offset += sizeof(float);
        }

        void deserializeField(const std::vector<uint8_t>& _data, size_t& _offset, std::string& _value)
        {
            TME_ASSERT_REF_PTR_OR_COPIABLE(_data);
            TME_ASSERT_REF_PTR_OR_COPIABLE(_value);

            uint32_t size = *reinterpret_cast<const uint32_t*>(&_data[_offset]);
            _offset += sizeof(uint32_t);
            _value.assign(reinterpret_cast<const char*>(&_data[_offset]), size);
            _offset += size;
        }
    }
}
