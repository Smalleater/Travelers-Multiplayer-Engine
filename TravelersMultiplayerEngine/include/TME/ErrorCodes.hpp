#ifndef TME_ERROR_CODES_HPP
#define TME_ERROR_CODES_HPP

#include "TME/Export.hpp"

namespace tme
{
    enum class ErrorCodes
    {
        Success = 0,
        PartialSuccess,
        AlreadyInitialized,
        Failure,

        CompletedWithErrors,
        NetworkEngineNotInitialized,

        InvalidSocket,
        ShutdownFailed,
        AddrInfoFailed,
        ConnectFailed,
        BindFailed,
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