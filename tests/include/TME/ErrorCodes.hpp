#ifndef TME_ERROR_CODES_HPP
#define TME_ERROR_CODES_HPP

#include "TME/Export.hpp"

namespace tme
{
    enum class ErrorCodes
    {
        Success = 0,
        PartialSuccess,
        Failure,

        AlreadyInitialized,

        ServerAlreadyStarted,
        ServerAlreadyStoped,

        ClientAlreadyConnected,
        ClientAlreadyDisconnected,

        EngineNotInitialized,
        ServerNotStarted,
        ClientNotConnected,

        ClientNotInitialized,
        ClientNotFound,

        NullSocket,

        InvalidSocket,
        ShutdownFailed,
        AddrInfoFailed,
        ConnectFailed,
        BindFailed,
        AcceptWouldBlock,
        AcceptFailed,
        ListenFailed,
        SendSizeTooLarge,
        SendConnectionClosed,
        SendFailed,
        ReceiveSizeTooLarge,
        ReceiveConnectionClosed,
        ReceiveWouldBlock,
        ReceiveFailed,
        SetBlockingFailed,
    };  
}

#endif