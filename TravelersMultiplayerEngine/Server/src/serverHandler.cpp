#include "TME/server/serverHandler.hpp"

namespace tme::server
{
    std::map<const char*, Server*> ServerHandler::m_servers;

    std::pair<ErrorCode, const Server*> ServerHandler::createServer(const char* _name)
    {
        if (m_servers.find(_name) != m_servers.end())
        {
            return { ErrorCode::ServerAlreadyExists, nullptr };
        }

        m_servers[_name] = new Server();

        return { ErrorCode::Success, m_servers[_name] };
    }

    std::pair<ErrorCode, const Server*> ServerHandler::getServer(const char* _name)
    {
        auto it = m_servers.find(_name);
        if (it != m_servers.end())
        {
            return { ErrorCode::Success, it->second };
        }

        return { ErrorCode::ServerNotFound, nullptr };
    }

    ErrorCode ServerHandler::deleteServer(const char* _name)
    {
        auto it = m_servers.find(_name);
        if (it == m_servers.end())
        {
            return ErrorCode::ServerNotFound;
        }

        it->second->Stop();

        delete it->second;
        m_servers.erase(it);

        return ErrorCode::Success;
    }
}