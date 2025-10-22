#ifndef TME_SERVER_SERVER_HANDLER_HPP
#define TME_SERVER_SERVER_HANDLER_HPP

#include "TME/export.hpp"

#include <utility>
#include <map>

#include "TME/errorCode.hpp"
#include "server.hpp"

namespace tme::server
{
    class ServerHandler
    {
    public:
        static TME_API std::pair<ErrorCode, const Server*> createServer(const char* _name);
        static TME_API std::pair<ErrorCode, const Server*> getServer(const char* _name);
        static TME_API ErrorCode deleteServer(const char* _name);

    private:
        static std::map<const char*, Server*> m_servers;
    };
}

#endif