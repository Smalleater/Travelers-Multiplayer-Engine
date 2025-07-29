#ifndef TME_NETWORK_MANAGER_HPP
#define TME_NETWORK_MANAGER_HPP

#include "TME/ErrorCodes.hpp"

namespace tme
{
    class NetworkManager
    {
    private:
        /* data */
    public:
        NetworkManager() {}
        ~NetworkManager() {}

        ErrorCodes StartServer();
        ErrorCodes StartClient();
    };    
}

#endif