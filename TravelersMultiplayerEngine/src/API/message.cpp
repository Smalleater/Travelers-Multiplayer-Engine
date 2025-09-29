#include "TME/message.hpp"

#include "core/messageFactory.hpp"

namespace tme
{
    namespace internal
    {
        void registerMessageType(const std::string& type, std::unique_ptr<Message>(*creator)(const std::vector<uint8_t>&))
        {
            tme::core::MessageFactory::registerMessage(type, creator);
        }
    }
}
