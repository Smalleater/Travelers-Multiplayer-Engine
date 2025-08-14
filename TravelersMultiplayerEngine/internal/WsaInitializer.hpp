#ifndef TME_WSA_INITIALIZER_HPP
#define TME_WSA_INITIALIZER_HPP

#ifdef _WIN32

#include <WinSock2.h>

#include "TME/ErrorCodes.hpp"

namespace tme
{
    /// @brief Helper class to initialize and clean up Winsock.
    ///
    /// This class encapsulates the initialization and cleanup of the Winsock library
    /// using RAII. It prevents copy and assignement to avoid multiple initializations
    /// or cleanups in the same process.
    class WsaInitializer
    {
    private:
        /// @brief Structure to hold Winsock initialization data.
        WSADATA m_wsaData;

        int m_wsaStartupResult = 0;

    public:
        /// @brief Default constructor. 
        /// Does not initialize Winsock; call Init() explicitly.
        WsaInitializer() {};

        /// @brief Destructor. 
        /// Automatically calls WSACleanup() to release Winsock resources.
        ~WsaInitializer();

        /// @brief Deleted copy constructor to prevent copying.
        WsaInitializer(const WsaInitializer&) = delete;

        /// @brief Deleted copy assignement operator to prevent assignement.
        WsaInitializer& operator=(const WsaInitializer&) = delete;

        /// @brief Initializes the Winsock library.
        /// @return ErrorCodes::Success on success, or ErrorCodes::Failure on error.
        ErrorCodes Init();

        int GetWsaStartupResult() const;
    };
}

#endif

#endif